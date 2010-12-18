/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

/* -*- mode: c-non-suck; -*- */

#include <cbf/primitive_controller.h>
#include <cbf/types.h>
#include <cbf/debug_macros.h>
#include <cbf/exceptions.h>
#include <cbf/xml_factory.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <vector>
#include <iostream>
#include <cassert>


namespace CBF {
	void PrimitiveController::init(
			Float coefficient,
			std::vector<ConvergenceCriterionPtr> convergence_criteria,
			ReferencePtr reference,
			PotentialPtr potential,
			SensorTransformPtr sensor_transform,
			EffectorTransformPtr effector_transform,
			std::vector<PrimitiveControllerPtr> subordinate_controllers,
			CombinationStrategyPtr combination_strategy,
			ResourcePtr resource
	) {
		m_Converged = false;

		m_Reference = reference;
		m_SubordinateControllers = subordinate_controllers;
		m_SensorTransform = sensor_transform;
		m_Potential = potential;
		m_EffectorTransform = effector_transform;
		m_CombinationStrategy = combination_strategy;
		m_Resource = resource;
		m_Coefficient = coefficient;

		for (unsigned int i = 0; i < m_SubordinateControllers.size(); ++i) { 
			m_SubordinateControllers[i]->m_Resource = m_Resource; 
			m_SubordinateControllers[i]->check_dimensions();
		}
		check_dimensions();

	}

	PrimitiveController::PrimitiveController(
		Float alpha,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		PotentialPtr potential,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<PrimitiveControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy,
		ResourcePtr resource
	)	{
		init(
			alpha,
			convergence_criteria,
			reference,
			potential,
			sensor_transform,
			effector_transform,
			subordinate_controllers,
			combination_strategy,
			resource
		);

		CBF_DEBUG("init");
	}
	

	void PrimitiveController::check_dimensions() {
		CBF_DEBUG("Reference and Potential dimensions " << m_Reference->dim() << " " << m_Potential->dim());

		if (m_Reference->dim() != m_Potential->dim())
			throw std::runtime_error("Reference and Potential dimensions mismatch");
	}	
	
	
	
	void PrimitiveController::do_update(int cycle) 
	{
		assert(m_Reference.get() != 0);
		assert(m_SensorTransform.get() != 0);
		assert(m_EffectorTransform.get() != 0);
		assert(m_Potential.get() != 0);
		assert(m_CombinationStrategy.get() != 0);

		assert(m_Resource.get() != 0);

		assert(m_Reference->dim() == m_Potential->dim());

		m_Reference->update();

		m_References = m_Reference->get();

		// CBF_DEBUG("ref: " << m_References[0])
	
		//! Update resource if nessecary
		m_Resource->update();

		//! Fill vector with data from sensor transform
		m_SensorTransform->update(m_Resource->get());
		m_EffectorTransform->update(m_Resource->get(), m_SensorTransform->task_jacobian());
	
		m_CurrentTaskPosition = m_SensorTransform->result();
		CBF_DEBUG("currentTaskPosition: " << m_CurrentTaskPosition);
	
		if (m_References.size() != 0) {	
			CBF_DEBUG("have reference!");
			//! then we do the gradient step
			m_Potential->gradient(m_GradientStep, m_References, m_CurrentTaskPosition);
			CBF_DEBUG("gradientStep: " << m_GradientStep);
 
			//! Map gradient step into resource step via exec:
			CBF_DEBUG("calling m_EffectorTransform->exec(): Type is: " << CBF_UNMANGLE(*m_EffectorTransform.get()));
			m_EffectorTransform->exec(m_GradientStep, m_ResourceStep);
		} else {
			m_ResourceStep = ublas::zero_vector<Float>(m_Resource->dim());
		}
	
		CBF_DEBUG("resourceStep: " << m_ResourceStep);
	
		//! then we recursively call subordinate controllers.. and gather their 
		//! effector transformed gradient steps.
		m_SubordinateGradientSteps.resize(m_SubordinateControllers.size());
	
		for (unsigned int i = 0; i < m_SubordinateControllers.size(); ++i) {
			m_SubordinateControllers[i]->update(cycle);
			m_SubordinateGradientSteps[i] = m_SubordinateControllers[i]->result();
			CBF_DEBUG("subordinate_gradient_step: " << m_SubordinateGradientSteps[i]);
		}
	
		m_CombinedResults = ublas::zero_vector<Float>(m_Resource->dim());
	
		m_CombinationStrategy->exec(m_CombinedResults, m_SubordinateGradientSteps);
	
		//! finally the results of all subordinate controllers are projected
		//! into our nullspace.For this we need the task jacobian and its inverse. 
		//! We get these from the effector transforms.
		m_InvJacobianTimesJacobian = ublas::prod(
			m_EffectorTransform->inverse_task_jacobian(), 
			m_SensorTransform->task_jacobian()
		);
	
		//! The projector is (1 - J# J), so this is result = result - (J# J result)
		//! which can be expressed as result -= ...
		m_CombinedResults -= ublas::prod(
			m_InvJacobianTimesJacobian,
			m_CombinedResults
		);
		CBF_DEBUG("combined_results * beta: " << m_CombinedResults);
	
		m_Result = (m_ResourceStep * m_Coefficient) + m_CombinedResults;
	}
	
	void PrimitiveController::do_action(int cycle) {
		update(cycle);
		m_Resource->add(m_Result);
		m_Converged = check_convergence();
	}
	
	bool PrimitiveController::check_convergence() {
		m_Converged = false;

		for (unsigned int i = 0, max = m_ConvergenceCriteria.size(); i < max; ++i) {
			if (m_ConvergenceCriteria[i]->check_convergence(*this)) {
				m_Converged = true;
				return true;
			}
		}
#if 0
		Float stepnorm = ublas::norm_2(m_Result);

		if (m_Reference->get().size() == 0)
			return false;

		Float min_dist = m_Potential->distance(m_CurrentTaskPosition, m_Reference->get()[0]);
		for (unsigned int i = 1, len = m_Reference->get().size(); i < len; ++i) {
			Float cur_dist = 	m_Potential->distance(m_CurrentTaskPosition, m_Reference->get()[i]);
			if (cur_dist < min_dist) min_dist = cur_dist;
		}

		Float dist = min_dist;

		CBF_DEBUG("distance_thresh " << m_TaskSpaceDistanceThreshold << " " << (dist < m_TaskSpaceDistanceThreshold) << " " << dist)
		CBF_DEBUG("stepnorm_thresh " << m_ResourceStepNormThreshold << " " << (stepnorm < m_ResourceStepNormThreshold) << " " << stepnorm)

		bool converged = 
			((m_ConvergenceCriterion & TASK_SPACE_DISTANCE_THRESHOLD) && (dist < m_TaskSpaceDistanceThreshold)) 
			||
			((m_ConvergenceCriterion & RESOURCE_STEP_THRESHOLD) && (stepnorm < m_ResourceStepNormThreshold));

		return converged;
#endif
		return false;
	}
	
	bool PrimitiveController::finished() {
		// check whether we have approached one of the references to within a small error
		return m_Converged;
		// return m_Potential->converged();
	}
	
	#ifdef CBF_HAVE_XSD
		PrimitiveController::PrimitiveController(const CBFSchema::PrimitiveController &xml_instance)
		{
			CBF_DEBUG("Constructing");
			if (xml_instance.Name())
				m_Name = *xml_instance.Name();

			Float coefficient = xml_instance.Coefficient();

			std::vector<ConvergenceCriterionPtr> criteria;

			for (
				::CBFSchema::PrimitiveController::ConvergenceCriterion_const_iterator it = 
					xml_instance.ConvergenceCriterion().begin();
				it != xml_instance.ConvergenceCriterion().end();
				++it
			) {
				ConvergenceCriterionPtr criterion = XMLObjectFactory::instance()->create<ConvergenceCriterion>(*it);
				criteria.push_back(criterion);
#if 0
				const ::CBFSchema::TaskSpaceDistanceThreshold *d = dynamic_cast<const ::CBFSchema::TaskSpaceDistanceThreshold *>(&(*it));
				if (d != 0) {
					m_ConvergenceCriterion |= TASK_SPACE_DISTANCE_THRESHOLD;
					m_TaskSpaceDistanceThreshold = d->Threshold();
					CBF_DEBUG("TaskSpaceDistanceThreshold " << m_TaskSpaceDistanceThreshold)
					continue;
				}

				const ::CBFSchema::ResourceStepNormThreshold *s = dynamic_cast<const ::CBFSchema::ResourceStepNormThreshold *>(&(*it));
				if (s != 0) {
					m_ConvergenceCriterion |= RESOURCE_STEP_THRESHOLD;
					m_ResourceStepNormThreshold = s->Threshold();
					CBF_DEBUG("ResourceStepThreshold " << m_ResourceStepNormThreshold)
					continue;
				}
				CBF_THROW_RUNTIME_ERROR("ConvergenceCriterion type not supported yet")
#endif
			}


			CBF_DEBUG("Creating reference...");
			ReferencePtr ref = 
				XMLObjectFactory::instance()->create<Reference>(xml_instance.Reference());

		
			//! Instantiate the potential
			CBF_DEBUG("Creating potential...");
			// m_Potential = XMLObjectFactory<Potential, CBFSchema::Potential>::instance()->create(xml_instance.Potential());
			PotentialPtr pot = 
				XMLObjectFactory::instance()->create<Potential>(xml_instance.Potential());

			//! Instantiate the Effector transform
			CBF_DEBUG("Creating sensor transform...");
			SensorTransformPtr sens = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.SensorTransform());
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Instantiate the Effector transform
			CBF_DEBUG("Creating effector transform...");
			EffectorTransformPtr eff = XMLObjectFactory::instance()->create<EffectorTransform>(xml_instance.EffectorTransform());
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Create a resource if given...
			CBF_DEBUG("Creating resource...");
			ResourcePtr res = XMLObjectFactory::instance()->create<Resource>(xml_instance.Resource());
			//! And bind to it...
			CBF_DEBUG("Binding to resource...");

			CBF_DEBUG("Creating combination strategy...");
			CombinationStrategyPtr com = 
				XMLObjectFactory::instance()->create<CombinationStrategy>(xml_instance.CombinationStrategy());

			//! Instantiate the subordinate controllers
			CBF_DEBUG("Creating subordinate controller(s)...");
			std::vector<PrimitiveControllerPtr> subs;
			for (
				::CBFSchema::PrimitiveController::PrimitiveController1_const_iterator it = xml_instance.PrimitiveController1().begin(); 
				it != xml_instance.PrimitiveController1().end();
				++it
			)
			{
				CBF_DEBUG("Creating subordinate controller...");
				CBF_DEBUG("------------------------");
				//! First we see whether we can construct a controller from the xml_document
				PrimitiveControllerPtr controller = XMLObjectFactory::instance()->create<PrimitiveController>(*it);
				controller->m_Resource = m_Resource;
 				subs.push_back(controller);

				CBF_DEBUG("------------------------");
			}

			init(
				coefficient, 
				criteria, 
				ref, 
				pot, 
				sens, 
				eff, 
				subs, 
				com, 
				res
			);
		}

		static XMLDerivedFactory<PrimitiveController, ::CBFSchema::PrimitiveController> x;
		
	#endif

} // namespace 



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
#include <cbf/generic_transform.h>
#include <cbf/plugin_macros.h>
#include <cbf/plugin_pool.h>
#include <cbf/dummy_reference.h>
#include <cbf/exceptions.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <vector>
#include <iostream>
#include <cassert>


namespace CBF {
	void PrimitiveController::init() {
		m_Converged = false;
	}

	PrimitiveController::PrimitiveController(
		ReferencePtr reference,
		PotentialPtr potential,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		ResourcePtr resource,
		CombinationStrategyPtr combination_strategy,
		std::vector<PrimitiveControllerPtr> subordinate_controllers,
		Float alpha
	) :
		m_Reference(reference),
		m_SubordinateControllers(subordinate_controllers),
		m_SensorTransform(sensor_transform),
		m_Potential(potential),
		m_EffectorTransform(effector_transform),
		m_CombinationStrategy(combination_strategy),
		m_Coefficient(alpha)
	{
		CBF_DEBUG("Constructor1")

		m_EffectorTransform->set_sensor_transform(sensor_transform);

		m_SensorTransform->set_resource(resource);
		m_EffectorTransform->set_resource(resource);

		check_dimensions();
	}
	

	void PrimitiveController::check_dimensions() {
		CBF_DEBUG("Reference and Potential dimensions " << m_Reference->dim() << " " << m_Potential->dim())

		if (m_Reference->dim() != m_Potential->dim())
			throw std::runtime_error("Reference and Potential dimensions mismatch");

		if (m_SensorTransform->task_dim() != m_Potential->dim())
			throw std::runtime_error("SensorTransform and Potential dimensions mismatch");

		if (m_SensorTransform->resource_dim() != m_SensorTransform->resource()->dim())
			throw std::runtime_error("SensorTransform and Resource dimensions mismatch");

		if (m_EffectorTransform->task_dim() != m_Potential->dim())
			throw std::runtime_error("EffectorTransform and Potential dimensions mismatch");

		if (m_EffectorTransform->resource_dim() != m_EffectorTransform->resource()->dim())
			throw std::runtime_error("EffectorTransform and Resource dimensions mismatch");
	}	
	
	
	
	void PrimitiveController::do_update(int cycle) 
	{
		assert(m_Reference.get() != 0);
		assert(m_SensorTransform.get() != 0);
		assert(m_EffectorTransform.get() != 0);
		assert(m_Potential.get() != 0);
		assert(m_CombinationStrategy.get() != 0);

		assert(m_SensorTransform->resource() != 0);
		assert(m_EffectorTransform->resource() != 0);

		assert(m_Reference->dim() == m_Potential->dim());

		m_Reference->update();

		m_References = m_Reference->get();

		// CBF_DEBUG("ref: " << m_References[0])
	
		//! Update resource if nessecary
		m_SensorTransform->resource()->update();
	
		//! Fill vector with data from sensor transform
		m_SensorTransform->update();
		m_EffectorTransform->update();
	
		m_CurrentTaskPosition = m_SensorTransform->result();
		CBF_DEBUG("currentTaskPosition: " << m_CurrentTaskPosition)
	
		if (m_References.size() != 0) {	
			CBF_DEBUG("No reference!");
			//! then we do the gradient step
			m_Potential->gradient(m_GradientStep, m_References, m_CurrentTaskPosition);
			CBF_DEBUG("gradientStep: " << m_GradientStep)
 
			//! Map gradient step into resource step via exec:
			m_EffectorTransform->exec(m_GradientStep, m_ResourceStep);
		} else {
			m_ResourceStep = ublas::zero_vector<Float>(m_EffectorTransform->resource_dim());
		}
	
		CBF_DEBUG("resourceStep: " << m_ResourceStep)
	
		//! then we recursively call subordinate controllers.. and gather their 
		//! effector transformed gradient steps.
		m_SubordinateGradientSteps.resize(m_SubordinateControllers.size());
	
		for (unsigned int i = 0; i < m_SubordinateControllers.size(); ++i) {
			m_SubordinateControllers[i]->update(cycle);
			m_SubordinateGradientSteps[i] = m_SubordinateControllers[i]->result();
			CBF_DEBUG("subordinate_gradient_step: " << m_SubordinateGradientSteps[i])
		}
	
		m_CombinedResults = ublas::zero_vector<Float>(m_EffectorTransform->resource_dim());
	
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
		CBF_DEBUG("combined_results * beta: " << m_CombinedResults)
	
		m_Result = (m_ResourceStep * m_Coefficient) + m_CombinedResults;
	}
	
	void PrimitiveController::do_action(int cycle) {
		update(cycle);

		m_EffectorTransform->resource()->add(m_Result);

		m_Converged = check_convergence();
	}
	
	bool PrimitiveController::check_convergence() {
		Float stepnorm = ublas::norm_2(m_Result);

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
	}
	
	bool PrimitiveController::finished() {
		// check whether we have approached one of the references to within a small error
		return m_Converged;
		// return m_Potential->converged();
	}
	
	#ifdef CBF_HAVE_XSD
		PrimitiveController::PrimitiveController(const PrimitiveControllerType &xml_instance) :
			m_Coefficient(1.0)
		
		{
			CBF_DEBUG("Constructing")
			init();


			if (xml_instance.Name().present())
				m_Name = *xml_instance.Name();

			m_Coefficient = xml_instance.Coefficient();
		
			m_ConvergenceCriterion = 0;

			if (xml_instance.ConvergenceCriterion().begin() == xml_instance.ConvergenceCriterion().end())
				CBF_THROW_RUNTIME_ERROR("Missing ConvergenceCriterion");

			for (
				PrimitiveControllerType::ConvergenceCriterion_const_iterator it = 
					xml_instance.ConvergenceCriterion().begin();
				it != xml_instance.ConvergenceCriterion().end();
				++it
			) {
				const TaskSpaceDistanceThresholdType *d = dynamic_cast<const TaskSpaceDistanceThresholdType *>(&(*it));
				if (d != 0) {
					m_ConvergenceCriterion |= TASK_SPACE_DISTANCE_THRESHOLD;
					m_TaskSpaceDistanceThreshold = d->Threshold();
					CBF_DEBUG("TaskSpaceDistanceThreshold " << m_TaskSpaceDistanceThreshold)
					continue;
				}
				const ResourceStepNormThresholdType *s = dynamic_cast<const ResourceStepNormThresholdType *>(&(*it));
				if (s != 0) {
					m_ConvergenceCriterion |= RESOURCE_STEP_THRESHOLD;
					m_ResourceStepNormThreshold = s->Threshold();
					CBF_DEBUG("ResourceStepThreshold " << m_ResourceStepNormThreshold)
					continue;
				}
				CBF_THROW_RUNTIME_ERROR("ConvergenceCriterion type not supported yet")
			}
		
			//! Instantiate the potential
			CBF_DEBUG("Creating potential...");
			m_Potential = PluginPool<Potential>::get_instance()->create_from_xml(xml_instance.Potential());

			//! Instantiate the Effector transform
			CBF_DEBUG("Creating sensor transform...")
			m_SensorTransform = PluginPool<SensorTransform>::get_instance()->create_from_xml(xml_instance.SensorTransform());
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Instantiate the Effector transform
			CBF_DEBUG("Creating effector transform...")
			m_EffectorTransform = PluginPool<EffectorTransform>::get_instance()->create_from_xml(xml_instance.EffectorTransform());
			m_EffectorTransform->set_sensor_transform(m_SensorTransform);
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Instantiate the subordinate controllers
			CBF_DEBUG("Creating subordinate controller(s)...")
			for (
				PrimitiveControllerType::PrimitiveController_const_iterator it = xml_instance.PrimitiveController().begin(); 
				it != xml_instance.PrimitiveController().end();
				++it
			)
			{
				CBF_DEBUG("Creating subordinate controller...")
				CBF_DEBUG("------------------------")
				//! First we see whether we can construct a controller from the xml_document
				ControllerPtr controller = PluginPool<Controller>::get_instance()->create_from_xml(*it);
				if (controller.get() == 0) {
					throw std::logic_error("This should not happen");
				} else {
					//! Then, since we only accept primitive controllers as subordinate controllers,
					//! let's see if it _is_ a primitive controller
					PrimitiveControllerPtr p_controller;
					p_controller = boost::dynamic_pointer_cast<PrimitiveController>(controller);
	
					if (p_controller.get() == 0)
						throw std::runtime_error("This should not happen. Controller was not convertible to PrimitiveController");
					else
		 				m_SubordinateControllers.push_back(p_controller);
				}

				CBF_DEBUG("------------------------")
			}
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Create a resource if given...
			CBF_DEBUG("Creating resource...")
			ResourcePtr res = PluginPool<Resource>::get_instance()->create_from_xml(xml_instance.Resource());
			//! And bind to it...
			CBF_DEBUG("Binding to resource...")

			m_SensorTransform->set_resource(res);
			m_EffectorTransform->set_resource(res);
		
			//! Create a resource if given...
			CBF_DEBUG("Creating combination strategy...")
			m_CombinationStrategy = 
				PluginPool<CombinationStrategy>::get_instance()->create_from_xml(xml_instance.CombinationStrategy());
		
			//! Create a resource if given...
			CBF_DEBUG("Creating reference...")
			m_Reference = 
				PluginPool<Reference>::get_instance()->create_from_xml(xml_instance.Reference());

			check_dimensions();
		}
		
	#endif
	
	CBF_PLUGIN_CLASS(PrimitiveController,Controller)
} // namespace 



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

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <vector>
#include <iostream>
#include <cassert>


namespace CBF {
	void PrimitiveController::init() {

	}

	PrimitiveController::PrimitiveController(
		PotentialPtr potential,
		EffectorTransformPtr effector_transform,
		SensorTransformPtr sensor_transform,
		CombinationStrategyPtr combination_strategy,
		std::vector<PrimitiveControllerPtr> subordinate_controllers,
		Float alpha,
		Float beta,
		bool init_reference_from_sensor_transform 
	) :
		m_SubordinateControllers(subordinate_controllers),
		m_SensorTransform(sensor_transform),
		m_Potential(potential),
		m_EffectorTransform(effector_transform),
		m_CombinationStrategy(combination_strategy),
		m_Coefficient(alpha),
		m_SubordinateCoefficient(beta),
		m_InitReferenceFromSensorTransform(init_reference_from_sensor_transform)
	{
		CBF_DEBUG("[PrimitiveController]: Constructor1")
	}
	
	PrimitiveController::PrimitiveController(
		Float alpha,
		Float beta,
		bool init_reference_from_sensor_transform
	) :
		m_CombinationStrategy(CombinationStrategyPtr(new AddingStrategy)),
		m_Coefficient(alpha),
		m_SubordinateCoefficient(beta),
		m_InitReferenceFromSensorTransform(init_reference_from_sensor_transform)
	{ 
		CBF_DEBUG("Constructor1")
	}
	
	
	
	
	FloatVector &PrimitiveController::subordinate_step(ublas::vector<Float> &result) 
	{
		assert(m_Reference.get() != 0);
		assert(m_SensorTransform.get() != 0);
		assert(m_EffectorTransform.get() != 0);
		assert(m_Potential.get() != 0);
		assert(m_CombinationStrategy.get() != 0);

		assert(m_SensorTransform->resource() != 0);
		assert(m_EffectorTransform->resource() != 0);

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
	
		if (m_InitReferenceFromSensorTransform == true) {
			//! This is only supported for DummyReferences, so let's check whether we have one of those
	
			DummyReferencePtr ref = boost::dynamic_pointer_cast<DummyReference, Reference>(m_Reference);
			if (ref.get() == 0) throw std::runtime_error("Not a DummyReference. Cannot initialize from sensor transform");
	
			ref->references()[0] = m_CurrentTaskPosition;
			m_InitReferenceFromSensorTransform = false;
		}
	
		//! then we do the gradient step
		m_Potential->gradient(m_GradientStep, m_References, m_CurrentTaskPosition);
		CBF_DEBUG("gradientStep: " << m_GradientStep)
 
		//! Map gradient step into resource step via exec:
		m_EffectorTransform->exec(m_GradientStep, m_ResourceStep);
	
		CBF_DEBUG("resourceStep: " << m_ResourceStep)
	
		//! then we recursively call subordinate controllers.. and gather their 
		//! effector transformed gradient steps.
		m_SubordinateGradientSteps.resize(m_SubordinateControllers.size());
	
		for (unsigned int i = 0; i < m_SubordinateControllers.size(); ++i) {
			m_SubordinateControllers[i]->subordinate_step(m_SubordinateGradientSteps[i]);
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
		CBF_DEBUG("combined_results * beta: " << m_CombinedResults * m_SubordinateCoefficient)
	
		result = (m_ResourceStep * m_Coefficient) + (m_CombinedResults * m_SubordinateCoefficient);

		return result;

		// CBF_DEBUG("result: " << result)
// 		std::cout << "@: ";
// 		for (unsigned int i = 0; i < result.size(); ++i) 
// 			std::cout << result[i] << " ";
// 		std::cout << std::endl;
	}
	
	bool PrimitiveController::step() {
		// a place to store our results
		ublas::vector<Float> result = ublas::zero_vector<Float>(m_SensorTransform->task_dim());
	
		subordinate_step(result);
		m_EffectorTransform->resource()->add(result);
	
		return finished();
		//CBF_DEBUG("Resource: " <<  m_EffectorTransform->resource()->get())
	}
	
	void PrimitiveController::set_resource(ResourcePtr resource) throw (std::runtime_error) {
		//CBF_DEBUG(m_SensorTransform.get())
		m_SensorTransform->set_resource(resource);
		m_EffectorTransform->set_resource(resource);

		for (unsigned int i = 0; i  < m_SubordinateControllers.size(); ++i)
			m_SubordinateControllers[i]->set_resource(resource);
	}
	
	bool PrimitiveController::finished() {
		// check whether we have approached one of the references to within a small error
		return m_Potential->converged();
	}
	
	#ifdef CBF_HAVE_XSD
		PrimitiveController::PrimitiveController(const PrimitiveControllerType &xml_instance) :
			m_Coefficient(1.0),
			m_SubordinateCoefficient(1.0),
			m_InitReferenceFromSensorTransform(false)
		
		{
			CBF_DEBUG("Constructing")
		
			if (xml_instance.Coefficient().present())
				m_Coefficient = *xml_instance.Coefficient();
		
			if (xml_instance.SubordinateCoefficient().present())
				m_SubordinateCoefficient = *xml_instance.SubordinateCoefficient();
		
			//! Instantiate the potential
			CBF_DEBUG("Creating potential...");
			if (xml_instance.Potential().present()) {
				CBF_DEBUG("Potential present")
				m_Potential = PluginPool<Potential>::get_instance()->create_from_xml(*xml_instance.Potential());
			}

			//! Instantiate the Effector transform
			if (xml_instance.SensorTransform().present()) {
				CBF_DEBUG("Creating sensor transform...")
				m_SensorTransform = PluginPool<SensorTransform>::get_instance()->create_from_xml(*xml_instance.SensorTransform());
			}
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Instantiate the Effector transform
			if (xml_instance.EffectorTransform().present()) {
				CBF_DEBUG("Creating effector transform...")
				m_EffectorTransform = PluginPool<EffectorTransform>::get_instance()->create_from_xml(*xml_instance.EffectorTransform());
				m_EffectorTransform->set_sensor_transform(m_SensorTransform);
			}
		
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
			if (xml_instance.Resource().present())
			{
				CBF_DEBUG("Creating resource...")
				ResourcePtr res = PluginPool<Resource>::get_instance()->create_from_xml(*xml_instance.Resource());
				//! And bind to it...
				CBF_DEBUG("Binding to resource...")
		
				//CBF_DEBUG(m_SensorTransform.get())
		
				set_resource(res);
			}
		
			//! Create a resource if given...
			CBF_DEBUG("Creating combination strategy...")
			if (xml_instance.CombinationStrategy().present())
				m_CombinationStrategy = 
					PluginPool<CombinationStrategy>::get_instance()->create_from_xml(*xml_instance.CombinationStrategy());
			else
				m_CombinationStrategy = 
					CombinationStrategyPtr(new AddingStrategy);
		
			//! Create a resource if given...
			CBF_DEBUG("Creating reference...")
			if (xml_instance.Reference().present())
				m_Reference = 
					PluginPool<Reference>::get_instance()->create_from_xml(*xml_instance.Reference());
			else
				m_Reference = 
					ReferencePtr(new DummyReference);
		}
		
	#endif
	
	CBF_PLUGIN_CLASS(PrimitiveController,Controller)
} // namespace 



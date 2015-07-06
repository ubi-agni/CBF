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
#include <cbf/xml_object_factory.h>

#include <boost/bind.hpp>

#include <vector>
#include <iostream>
#include <cassert>


namespace CBF {
	SubordinateController::SubordinateController(
		Float timestep,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		FilterPtr reference_filter,
		PotentialPtr potential,
		ErrorControlPtr error_control,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<SubordinateControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy
	)
	{
		init(
      timestep,
			convergence_criteria,
			reference,
      		reference_filter,
      		potential,
      		error_control,
			sensor_transform,
			effector_transform,
			subordinate_controllers,
			combination_strategy
		);

		CBF_DEBUG("init");
	}

	void SubordinateController::init(
		Float timestep,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		FilterPtr reference_filter,
		PotentialPtr potential,
		ErrorControlPtr error_control,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<SubordinateControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy
	) {
		m_Master = NULL;
		m_TimeStep = timestep;
		m_ConvergenceCriteria = convergence_criteria;
		m_Reference = reference;
		m_Potential = potential;
		m_ReferenceFilter = reference_filter,
		m_SensorTransform = sensor_transform;
		m_EffectorTransform = effector_transform;
		m_ErrorControl = error_control;
		m_SubordinateControllers = subordinate_controllers;
		m_CombinationStrategy = combination_strategy;

		for (std::vector<SubordinateControllerPtr>::iterator 
				  it  = m_SubordinateControllers.begin(),
				  end = m_SubordinateControllers.end(); 
			  it != end; ++it) {
			(*it)->m_Master = this;
		}

		// reference filter uses the potential's gradient() and integration() functions
		m_ReferenceFilter->diff = boost::bind(&Potential::gradient, m_Potential, _1, _2, _3);
		m_ReferenceFilter->integration = boost::bind(&Potential::integration, m_Potential, _1, _2, _3, _4);

		// resize variables
		m_CurrentTaskPosition = FloatVector(m_SensorTransform->sensor_dim());
		m_TaskStateError = FloatVector(m_SensorTransform->task_dim());
		m_TaskVelocity = FloatVector(m_SensorTransform->task_dim());
		m_ResourceVelocity = FloatVector(m_SensorTransform->resource_dim());

		m_CombinedResourceVlocity = FloatVector(m_SensorTransform->resource_dim());
		m_NullSpaceResourceVlocity = FloatVector(m_SensorTransform->resource_dim());

		m_NullSpaceMotion.resize(m_SubordinateControllers.size(), FloatVector::Zero(m_SensorTransform->resource_dim()));
	}

	void SubordinateController::reset(const FloatVector resource_value, const FloatVector resource_velocity)
	{
		// reset sensor transform
		m_SensorTransform->update(resource_value, resource_velocity);

		// reset error controller
		m_ErrorControl->reset();

		// reset reference
		std::vector<FloatVector> lRef = std::vector<FloatVector>(1, m_SensorTransform->result());
		lRef[0] = m_SensorTransform->result();
		m_Reference->set_references(lRef);

		// reset reference filter
		m_ReferenceFilter->reset(m_SensorTransform->result(),
		                         m_SensorTransform->get_task_velocity());

		// reset null motions
		for (std::vector<SubordinateControllerPtr>::iterator
			it  = m_SubordinateControllers.begin(),
			end = m_SubordinateControllers.end();
			it != end; ++it) {
			
			//(*it)->reset(resource_value, FloatVector::Zero(resource_value.size()));
			(*it)->reset(resource_value, resource_velocity);
		}
	}

	void SubordinateController::reset_sensor()
	{
		// reset sensor transform
		m_SensorTransform->update(resource_filter()->get_filtered_state(),
		                          resource_filter()->get_filtered_state_vel());

		// reset reference filter
		m_ReferenceFilter->reset(m_SensorTransform->result(),
		                         m_SensorTransform->get_task_velocity());

		// reset null motions
		for (std::vector<SubordinateControllerPtr>::iterator
			it  = m_SubordinateControllers.begin(),
			end = m_SubordinateControllers.end();
			it != end; ++it) {

			(*it)->reset_sensor();
		}
	}

	void PrimitiveController::primitive_init(ResourcePtr resource, FilterPtr resource_filter, LimiterPtr limiter)
	{
		m_Resource = resource;
		m_ResourceFilter = resource_filter;
		m_ResourceLimiter = limiter;

		reset(m_Resource->get_position(), m_Resource->get_velocity());

		check_dimensions();
	}

	void PrimitiveController::reset()
	{
		SubordinateController::reset(m_ResourceFilter->get_filtered_state(), m_ResourceFilter->get_filtered_state_vel());
	}

	void PrimitiveController::reset(const FloatVector resource_value, const FloatVector resource_velocity)
	{
		m_ResourceFilter->reset(resource_value, resource_velocity);

		SubordinateController::reset(resource_value, resource_velocity);
	}

	PrimitiveController::PrimitiveController(Float timestep,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		FilterPtr reference_filter,
		PotentialPtr potential,
		ErrorControlPtr error_control,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<SubordinateControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy,
		ResourcePtr resource,
		FilterPtr resource_filter,
		LimiterPtr limiter)	:
			SubordinateController(
			timestep,
			convergence_criteria,
			reference,
			reference_filter,
			potential,
			error_control,
			sensor_transform,
			effector_transform,
			subordinate_controllers,
			combination_strategy
		)
	{
		primitive_init(resource, resource_filter, limiter);

		CBF_DEBUG("init");
	}

	void SubordinateController::check_dimensions() {
		if (m_Reference->dim() != m_Potential->sensor_dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Reference and Potential dimensions mismatch: " << m_Reference->dim() << " is not equal to " << m_Potential->sensor_dim());

		if (m_SensorTransform->task_dim() != m_Potential->task_dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Sensor Transform and Potential dimension mismatch: " << m_SensorTransform->task_dim() << " is not equal to " << m_Potential->task_dim());

		if (m_SensorTransform->resource_dim() != m_EffectorTransform->resource_dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Sensor Transform and Effector transform resource dimension mismatch: " << m_SensorTransform->resource_dim() << " is not equal to " << m_EffectorTransform->resource_dim());

		if (m_SensorTransform->task_dim() != m_EffectorTransform->task_dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Sensor Transform and Effector transform task dimension mismatch: " << m_SensorTransform->task_dim() << " is not equal to " << m_EffectorTransform->task_dim());
	}	

	ResourcePtr SubordinateController::resource() { 
		return m_Master->resource(); 
	}	

	FilterPtr SubordinateController::resource_filter() {
		return m_Master->resource_filter();
	}

	void PrimitiveController::update(Float timestep) {
		m_Resource->read();
		m_ResourceFilter->update(m_Resource->get_position(), m_Resource->get_velocity(), timestep);

		SubordinateController::update(timestep);
	}	
	
	void SubordinateController::update(Float timestep)
	{
		assert(m_Reference.get() != 0);
		assert(m_ReferenceFilter.get() != 0);
		assert(m_SensorTransform.get() != 0);
		assert(m_EffectorTransform.get() != 0);
		assert(m_Potential.get() != 0);
		assert(m_CombinationStrategy.get() != 0);

		if(timestep < m_TimeStep*0.1) {
			CBF_THROW_RUNTIME_ERROR(m_Name << ": controller update timestep (" << timestep << ") is too small to compare to the default time step (" << m_TimeStep << ")!!");
		}

		//! Fill vector with data from sensor transform
		m_SensorTransform->update(resource_filter()->get_filtered_state(),
		                          resource_filter()->get_filtered_state_vel());

		CBF_DEBUG("jacobian: " << m_SensorTransform->task_jacobian());

		m_EffectorTransform->update(resource_filter()->get_filtered_state(), m_SensorTransform->task_jacobian());
		CBF_DEBUG("inv. jacobian: " << m_EffectorTransform->inverse_task_jacobian());
	
		m_CurrentTaskPosition = m_SensorTransform->result();
		CBF_DEBUG("currentTaskPosition: " << m_CurrentTaskPosition);

		m_Reference->update();
		m_References = m_Reference->get();

		if (m_References.size() != 0) {	

			// reference filtering
			m_ReferenceFilter->update(
				m_Potential->select_reference(m_References, m_ReferenceFilter->get_filtered_state()),
				FloatVector::Zero(m_Potential->task_dim()),
				timestep);

			m_ReferenceFilter->diff(
				m_TaskStateError,
				m_ReferenceFilter->get_filtered_state(),
				m_CurrentTaskPosition);

			// current task velocity
			m_TaskVelocity = m_SensorTransform->get_task_velocity();

			// control
			m_ErrorControl->update(
				m_TaskStateError,
				m_ReferenceFilter->get_filtered_state_vel()-m_TaskVelocity,
				timestep);

			m_TaskVelocity = m_ErrorControl->get();

			// compute resource velocity from task space velocity
			m_EffectorTransform->update(resource_filter()->get_filtered_state(), m_SensorTransform->task_jacobian());
			m_EffectorTransform->exec(m_TaskVelocity, m_ResourceVelocity);
		} else {
			m_ResourceVelocity.setZero();
		}
	
		// Null space motion control
		m_NullSpaceResourceVlocity.setZero();
		if (m_SubordinateControllers.size()>0) {
			// then we recursively call subordinate controllers.. and gather their
			// effector transformed gradient steps.
			for (unsigned int i = 0; i < m_SubordinateControllers.size(); ++i) {
				m_SubordinateControllers[i]->update(timestep);
				m_NullSpaceMotion[i] = m_SubordinateControllers[i]->result_resource_velocity();
				CBF_DEBUG("NullSpace motion : " << m_NullSpaceMotion[i]);
			}

			m_CombinationStrategy->exec(m_NullSpaceResourceVlocity, m_NullSpaceMotion);

			// finally the results of all subordinate controllers are projected
			// into our nullspace.For this we need the task jacobian and its inverse.
			// We get these from the effector transforms.
			m_InvJacobianTimesJacobian = m_EffectorTransform->inverse_task_jacobian()
			                           * m_SensorTransform->task_jacobian();

			// The projector is (1 - J# J), so this is result = result - (J# J result)
			// which can be expressed as result -= ...
			m_NullSpaceResourceVlocity -= m_InvJacobianTimesJacobian
			                            * m_NullSpaceResourceVlocity;

			CBF_DEBUG("combined_results * beta: " << m_NullSpaceResourceVlocity);
		}
	
		m_CombinedResourceVlocity = m_ResourceVelocity + m_NullSpaceResourceVlocity;
	}
	
	void PrimitiveController::action(Float timestep) {

		if(timestep < m_TimeStep*0.1) {
			CBF_THROW_RUNTIME_ERROR(m_Name << ": controller update timestep (" << timestep << ") is too small to compare to the average time step (" << m_TimeStep << ")!!");
		}

		FloatVector lRes = FloatVector::Zero(m_CombinedResourceVlocity.size());
		lRes = m_ResourceFilter->get_filtered_state()+m_CombinedResourceVlocity*timestep;

		m_ResourceLimiter->limit(m_ResourceFilter->get_filtered_state(),
		                         m_ResourceFilter->get_filtered_state_vel(),
		                         lRes,
		                         m_CombinedResourceVlocity);

		m_Resource->write(m_CombinedResourceVlocity, timestep);

		m_Converged = check_convergence();
	}
	
	bool SubordinateController::check_convergence() {
		m_Converged = false;

		for (unsigned int i = 0, max = m_ConvergenceCriteria.size(); i < max; ++i) {
			if (m_ConvergenceCriteria[i]->check_convergence(*this)) {
				m_Converged = true;
				break;
			}
		}
		m_Stalled = !m_Converged && m_Result.norm() < 0.005 * m_GradientStep.norm();
		return m_Converged;
	}
	
	bool SubordinateController::finished() {
		// check whether we have approached one of the references to within a small error
		return m_Converged;
	}

	void PrimitiveController::check_dimensions() const {
		SubordinateController::check_dimensions();

		if (m_SensorTransform->resource_dim() != m_Resource->dim())
			CBF_THROW_RUNTIME_ERROR(m_Name + ": Sensor Transform and Resource dimension mismatch: " << m_SensorTransform->resource_dim() << " is not equal to " << m_Resource->dim());

		if (m_EffectorTransform->resource_dim() != m_Resource->dim())
			CBF_THROW_RUNTIME_ERROR(m_Name + ": Effector Transform and Resource dimension mismatch: " << m_EffectorTransform->resource_dim() << " is not equal to " << m_Resource->dim());
	}	

	
	#ifdef CBF_HAVE_XSD
		SubordinateController::SubordinateController(const CBFSchema::SubordinateController &xml_instance, ObjectNamespacePtr object_namespace) :
			Controller(xml_instance, object_namespace)
		{
			CBF_DEBUG("Constructing");
			if (xml_instance.Name())
				m_Name = *xml_instance.Name();

			Float timestep = xml_instance.Coefficient();

			std::vector<ConvergenceCriterionPtr> convergence_criteria;

			for (
				::CBFSchema::SubordinateController::ConvergenceCriterion_const_iterator it = 
					xml_instance.ConvergenceCriterion().begin();
				it != xml_instance.ConvergenceCriterion().end();
				++it
			) {
				CBF_DEBUG("adding criterion");
				ConvergenceCriterionPtr criterion = XMLObjectFactory::instance()->create<ConvergenceCriterion>(*it, object_namespace);
				convergence_criteria.push_back(criterion);
			}


			CBF_DEBUG("Creating reference...");
			ReferencePtr reference = 
				XMLObjectFactory::instance()->create<Reference>(xml_instance.Reference(), object_namespace);

		
			//! Instantiate the potential
			CBF_DEBUG("Creating potential...");
			// m_Potential = XMLObjectFactory<Potential, CBFSchema::Potential>::instance()->create(xml_instance.Potential());
			PotentialPtr potential = 
				XMLObjectFactory::instance()->create<Potential>(xml_instance.Potential(), object_namespace);

      		//! Instantiate the taskspace planner
			CBF_DEBUG("Creating task space planner...");
			FilterPtr reference_filter;
			//TaskSpacePlannerPtr planner =
			//  XMLObjectFactory::instance()->create<TaskSpacePlanner>(xml_instance.TaskSpacePlanner(), object_namespace);

			//! Instantiate the Effector transform
			CBF_DEBUG("Creating sensor transform...");
			SensorTransformPtr sensor_transform = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.SensorTransform(), object_namespace);
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Instantiate the Effector transform
			CBF_DEBUG("Creating effector transform...");
			EffectorTransformPtr effector_transform = XMLObjectFactory::instance()->create<EffectorTransform>(xml_instance.EffectorTransform(), object_namespace);
			ErrorControlPtr error_control; // TODO

			CBF_DEBUG("Creating combination strategy...");
			CombinationStrategyPtr combination_strategy  = 
				XMLObjectFactory::instance()->create<CombinationStrategy>(xml_instance.CombinationStrategy(), object_namespace);

			//! Instantiate the subordinate controllers
			CBF_DEBUG("Creating subordinate controller(s)...");
			std::vector<SubordinateControllerPtr> subordinate_controllers;
			for (
				::CBFSchema::SubordinateController::SubordinateController1_const_iterator it = xml_instance.SubordinateController1().begin(); 
				it != xml_instance.SubordinateController1().end();
				++it
			)
			{
				CBF_DEBUG("Creating subordinate controller...");
				CBF_DEBUG("------------------------");
				//! First we see whether we can construct a controller from the xml_document
				SubordinateControllerPtr controller = XMLObjectFactory::instance()->create<SubordinateController>(*it, object_namespace);
 				subordinate_controllers.push_back(controller);
				CBF_DEBUG("------------------------");
			}

			init(
        		timestep,
				convergence_criteria,
				reference,
				reference_filter,
				potential,
				error_control,
				sensor_transform,
				effector_transform,
				subordinate_controllers,
				combination_strategy
			);
		}

		PrimitiveController::PrimitiveController(const CBFSchema::PrimitiveController &xml_instance, ObjectNamespacePtr object_namespace) :
			SubordinateController(xml_instance, object_namespace) {

			ResourcePtr res = XMLObjectFactory::instance()->create<Resource>(xml_instance.Resource(), object_namespace);
			FilterPtr res_filter; // TODO
			LimiterPtr res_limiter; // TODO

			primitive_init(res, res_filter, res_limiter);
		}


		static XMLDerivedFactory<PrimitiveController, ::CBFSchema::PrimitiveController> x;
		static XMLDerivedFactory<SubordinateController, ::CBFSchema::SubordinateController> x2;
		
	#endif

} // namespace 



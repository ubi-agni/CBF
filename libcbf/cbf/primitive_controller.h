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

#ifndef CONTROL_BASIS_PRIMITIVE_CONTROLLER_HH
#define CONTROL_BASIS_PRIMITIVE_CONTROLLER_HH

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <vector>

#include <cbf/controller.h>
#include <cbf/potential.h>
#include <cbf/resource.h>
#include <cbf/effector_transform.h>
#include <cbf/reference.h>
#include <cbf/sensor_transform.h>
#include <cbf/combination_strategy.h>

CBF_PLUGIN_PREAMBLE(PrimitiveController)

namespace CBF {
	class PrimitiveController;
	typedef boost::shared_ptr<PrimitiveController> PrimitiveControllerPtr;

	struct PrimitiveController;

	/**
		@brief The "primitive" controller is the 
		main tool to use in the control basis framework.
	
		A primitive controller can be "synthesized" 
		by combining an artificial potential function,
		a sensor transform, an effector transform 
		and a resource to act on.
	
		Additionally several subordinate controllers 
		can be specified which will try to reach 
		secondary goals. The result of the gradient 
		steps of the subordinate controller is 
		projected into the null space of this controller.
	*/
	struct PrimitiveController : public Controller {
		CBF_PLUGIN_DECL_METHODS(PrimitiveController)

		/** 
			@brief: Create a barebones controller which is 
			not useful in itself 
	
			Note that init_reference_from_sensor_transform 
			is only supported for DummyReferences		
		*/	
		PrimitiveController(
			Float coefficient = 1.0,
			bool init_reference_from_sensor_transform = false
		);
	
		/**
			@brief Create a controller with the members 
			set from the specified arguments
	
			Note that init_reference_from_sensor_transform 
			is only supported for DummyReferences		
		*/
		PrimitiveController(
			PotentialPtr potential,
			EffectorTransformPtr effector_transform,
			SensorTransformPtr sensor_transform,
			CombinationStrategyPtr combination_strategy = CombinationStrategyPtr(new AddingStrategy),
			std::vector<PrimitiveControllerPtr> subordinate_controllers = std::vector<PrimitiveControllerPtr>(),
			Float coefficient = 1.0,
			bool init_reference_from_sensor_transform = false
		);
	
	
		protected:
			bool m_Converged;

			/**
				@brief: Check for convergence of the controller
		
				This should only be used after the result has been calculated
			*/ 
			virtual bool check_convergence();
	
			/**
				DISTANCE_THRESHOLD means that the potential should signal
				convergence when only a certain distance away from at
				least one reference..
		
				Some potentials might not have a notion of reference. For 
				these convergence might be determined by a sufficiently
				small gradient step norm. For this the STEP_THRESHOLD 
				is useful.
			*/
			enum ConvergenceCriterion { TASK_SPACE_DISTANCE_THRESHOLD = 1, RESOURCE_STEP_THRESHOLD = 2 };
			unsigned int m_ConvergenceCriterion;
		
			Float m_TaskSpaceDistanceThreshold;
			Float m_ResourceStepNormThreshold;
		
			//* @brief Function for stuff common to all constructors */
			void init();

			/**
				A reference determines the task 
				space position that the controller
				tries to realize. Sometimes there 
				can be more than one possible 
				target. It's left to the controller 
				(by way of the potential
				function) which of the targets to reach..
			*/
			ReferencePtr m_Reference;
	
			/**
				A controller can have subordinate controllers whose control signal
				get projected into the nullspace of the task jacobian
			*/
			std::vector<PrimitiveControllerPtr> m_SubordinateControllers;
	
			/**
				The sensor transform is responsible for providing the feedback
				of this closed loop controller. See SensorTransform..
		
				The derivative of the sensor transform is the task jacobian. 
				Its inverse will be represented by the effector transform..
			*/
			SensorTransformPtr m_SensorTransform;
	
			/**
				The potential is the "heart" of the controller. 
			*/
			PotentialPtr m_Potential;
	
			/**
				The effector transform is responsible for mapping the 
				gradient step output of the controller to a real actuator.
		
				In many cases it will implement the pseudoinverse jacobian 
				of the sensortransform..
			*/
			EffectorTransformPtr m_EffectorTransform;
	
			CombinationStrategyPtr m_CombinationStrategy;

			/**
				The factor for the primary gradient step
			*/
			Float m_Coefficient;
	
			/**
				This bool tells the primitive controller to set the
				reference to the result of the sensor transform during the
				first run.
			*/
			bool m_InitReferenceFromSensorTransform;

		public:
			/**
				This function only returns sensible values after
				the controller has run at least once...
			*/
			const FloatVector &current_task_position() {
				return m_CurrentTaskPosition; }

			void set_reference(ReferencePtr reference) {
				m_Reference = reference; }
	
			ReferencePtr reference() { return m_Reference; }
	
			void set_subordinate_controllers(
				std::vector<PrimitiveControllerPtr> subordinate_controllers) {
				m_SubordinateControllers = subordinate_controllers; }
		
			std::vector<PrimitiveControllerPtr> &subordinate_controllers() {
				return m_SubordinateControllers;	}
	
			void set_sensor_transform(SensorTransformPtr sensor_transform) {
				m_SensorTransform = sensor_transform; }
	
			SensorTransformPtr sensor_transform() {
				return m_SensorTransform; }
	
			void set_potential(PotentialPtr potential) {
				m_Potential = potential; }
	
			PotentialPtr potential() {	return m_Potential; }
	
			void set_effector_transform(
				EffectorTransformPtr effector_transform) {
				m_EffectorTransform = effector_transform; }
	
			EffectorTransformPtr effector_transform() {
				return m_EffectorTransform; }
	
			void set_combination_strategy(
				CombinationStrategyPtr combination_strategy) {
				m_CombinationStrategy = combination_strategy; }
	
			CombinationStrategyPtr combination_strategy() {
				return m_CombinationStrategy; }
	
			void set_coefficient(Float coefficient);

			Float coefficient();
	
			void set_subordinate_coefficient(Float beta);

			Float subordinate_coefficient();
	
			/**
				This method might throw a std::runtime_error describing a dimension
				mismatch. 

				Also note that this binds both the sensor and the effector 
				transform to the same resource. If that is not what you want (e.g.
				your sensor and effector transform work with different resources, then
				use the setter methods of the sensor and effector transforms respectively.
			*/
			virtual void set_resource(ResourcePtr resource) throw (std::runtime_error);
		
			/**
				This reimplementation of the base class' method assumes that we are not a subordinate
				controller, because subordinate controllers are always called via the do_step() method
			*/
			virtual void do_update(int cycle);

			virtual void do_action(int cycle);

			/**
				@brief Returns the result of the calculationss done on update().

				The update() function is supposed to update all sensor transforms, potentials
				etc. The last step of a control cycly is applying the result (the resource
				space gradient step) of these	calculations to the resource.
			*/
			virtual FloatVector &result() { return m_Result; }

			/**

				Check if controller is converged. Call this function only
				after calling step() at least once..
			*/
			virtual bool finished();

			virtual void check_dimensions();

		protected:
			FloatVector m_Result;

			/**	Member variable for efficiency reasons.. */
			FloatMatrix m_TaskJacobian;
	
			/**	Member variable for efficiency reasons.. */
			FloatMatrix m_InverseTaskJacobian;
	
			/**	Member variable for efficiency reasons.. */
			FloatMatrix m_InvJacobianTimesJacobian;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_CurrentTaskPosition;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_GradientStep;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_ResourceStep;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_CombinedResults;
	
			/**	Member variable for efficiency reasons.. */
			std::vector<FloatVector> m_SubordinateGradientSteps;
	
			/**	Member variable for efficiency reasons.. */
			std::vector<FloatVector> m_References;
	};

} // namespace



#endif


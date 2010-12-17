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

#ifndef CONTROL_BASIS_EFFECTOR_TRANSFORM_HH
#define CONTROL_BASIS_EFFECTOR_TRANSFORM_HH

#include <cbf/resource.h>
#include <cbf/types.h>
#include <cbf/sensor_transform.h>
#include <cbf/object.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	
	class EffectorTransform;
	typedef boost::shared_ptr<EffectorTransform> EffectorTransformPtr;
	
	
	/**
		@brief The EffectorTransform takes a gradient step in task space
		and maps it to the resource's configuration space.
	*/
	struct EffectorTransform : public Object {

		EffectorTransform()
		{
	
		}

		/**
			A virtual destructor to make destruction of objects of derived types possible
			by deleting a pointer to the base class (this).
		*/	
		virtual ~EffectorTransform() {
	
		}
	
		/**
			@brief This function is called by a controller in each control cycle
	
			This method must be called before exec. Ideally right after the resource has 
			changed and the SensorTransfor has been read(). This method is meant for 
			EffectorTransforms to be able to do one-shot expensive computations whose 
			results will consequently be used by different methods, e.g. 
			the inverse task jacobian given that it depends on the current resource
			value / the current task jacobian (in the common case that it is not constant).
		*/
		virtual void update(
			const FloatVector &resource_value, 
			const FloatMatrix &task_jacobian
		) = 0;

	
		/**
			This purely virtual method must be implemented by subclasses. 
			Its result is then passed on to the resource by the controller.
	
			This method must map the input gradient step onto a step in resource space.
			This step is then added lateron to the current resource values by the
			controller.

			Call this only after calling update() to make sure the EffectorTransform
			is in a valid state..
		*/
		virtual void exec(
			const FloatVector &input,
			FloatVector &result
		) = 0;
	
		/**
			A way to get to the current task jacobian. This is needed by the
			controller to construct the nullspace projector (but for nothing 
			else. Implementations are thus free to do anything they want to 
			calculate the resource step update in exec())
	
			May only be called after a call to update() to update the internal
			matrices.
		*/
		virtual const FloatMatrix &inverse_task_jacobian() const { 
			return m_InverseTaskJacobian; 
		}
	
		virtual unsigned int task_dim() const { 
			return m_InverseTaskJacobian.size2();
		}

		virtual unsigned int resource_dim() const {
			return m_InverseTaskJacobian.size1();
		}

		protected:
			/**
				This should be calculated in the update() function. the inverse_task_jacobian() function
				should then return a reference to this to avoid unnessecary recomputations.
			*/
			FloatMatrix m_InverseTaskJacobian;
	};
} // namespace

#endif


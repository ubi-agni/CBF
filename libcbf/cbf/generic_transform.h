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

#ifndef CBF_GENERIC_TRANSFORM_H
#define CBF_GENERIC_TRANSFORM_H

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/utilities.h>

#include <boost/numeric/ublas/io.hpp>

namespace CBFSchema {
	class GenericEffectorTransform;
	class DampedGenericEffectorTransform;
	class DampedWeightedGenericEffectorTransform;
}

namespace CBF {

	/**
		@brief Pseudo inverse based generic effector transform (non-damped, non-weighted)
	*/
	struct GenericEffectorTransform : public EffectorTransform {
		GenericEffectorTransform (const CBFSchema::GenericEffectorTransform &xml_instance);

		GenericEffectorTransform() { }
	
		virtual void update();
	
		virtual void exec(const FloatVector &input, FloatVector &result) {
			result = ublas::prod(m_InverseTaskJacobian, input);
		}
	
		virtual unsigned resource_dim() const {
			return m_SensorTransform->resource_dim();
		}
	
		virtual unsigned int task_dim() const {
			return m_SensorTransform->task_dim();
		}
	};
	
	typedef boost::shared_ptr<GenericEffectorTransform> GenericEffectorTransformPtr;


	/**
		@brief Pseudo inverse based generic effector transform (non-damped, non-weighted)
	*/
	struct DampedGenericEffectorTransform : public EffectorTransform {
		DampedGenericEffectorTransform (const CBFSchema::DampedGenericEffectorTransform &xml_instnace);
	
		virtual void update();
	
		DampedGenericEffectorTransform(
			Float damping_constant = 0.1)
			: m_DampingConstant(damping_constant)
		{

		}
	
		virtual void exec(const FloatVector &input, FloatVector &result) {
			result = ublas::prod(m_InverseTaskJacobian, input);
		}
	
		virtual unsigned resource_dim() const {
			return m_SensorTransform->resource_dim();
		}
	
		virtual unsigned int task_dim() const {
			return m_SensorTransform->task_dim();
		}

		protected:
			Float m_DampingConstant;
	};
	
	typedef boost::shared_ptr<DampedGenericEffectorTransform> DampedGenericEffectorTransformPtr;

	
	/**
		@brief Pseudo inverse based generic effector transform (damped, weighted).

		TODO: Implement!!!
	*/
	struct DampedWeightedGenericEffectorTransform : public EffectorTransform {
		DampedWeightedGenericEffectorTransform (const CBFSchema::DampedWeightedGenericEffectorTransform &xml_instance);

		protected:	
	 		FloatMatrix m_Weights;

			/**
				@brief This variable controls the amount of damping

				Instead of 1/lambda where lambda is a singular value of the 
				sensor transform this uses lambda/(m_DampingConstant + lambda^2)
			*/
			Float m_DampingConstant;

		public:	
			virtual void update();
		
			DampedWeightedGenericEffectorTransform(
				Float dampingConstant = 0.1)
			{
				m_DampingConstant = dampingConstant;
			}
		
		
			virtual void exec(const FloatVector &input, FloatVector &result) {
				result = ublas::prod(m_InverseTaskJacobian, input);
			}
		
			virtual unsigned resource_dim() const {
				return m_SensorTransform->resource_dim();
			}
		
			virtual unsigned int task_dim() const {
				return m_SensorTransform->task_dim();
			}
	};
	
	typedef boost::shared_ptr<DampedWeightedGenericEffectorTransform> DampedWeightedGenericEffectorTransformPtr;
	
	
} // namespace

#endif


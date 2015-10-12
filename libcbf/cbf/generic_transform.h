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

#include <cbf/config.h>
#include <cbf/debug_macros.h>
#include <cbf/types.h>
#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/utilities.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <algorithm>

namespace CBFSchema {
	class GenericEffectorTransform;
	class DampedGenericEffectorTransform;
	class ThresholdGenericEffectorTransform;
	class PaddedEffectorTransform;
}

namespace CBF {

	/**
		@brief Pseudo inverse based generic effector transform (non-damped, non-weighted)
	*/
	struct GenericEffectorTransform : public EffectorTransform {
		GenericEffectorTransform (const CBFSchema::GenericEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace);

		GenericEffectorTransform(unsigned int task_dim, unsigned int resource_dim) 
		{ 
			init(task_dim, resource_dim);
		}
	
		virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian);
	
		virtual void exec(const FloatVector &input, FloatVector &result) {
			result = m_InverseTaskJacobian * input;
		}

		void init(unsigned int task_dim, unsigned int resource_dim) {
			m_InverseTaskJacobian = FloatMatrix((int) resource_dim, (int) task_dim);
		}	
	};
	
	typedef boost::shared_ptr<GenericEffectorTransform> GenericEffectorTransformPtr;


	/**
		@brief Pseudo inverse based generic effector transform (damped, non-weighted)
	*/
	struct DampedGenericEffectorTransform : public EffectorTransform {
		DampedGenericEffectorTransform (const CBFSchema::DampedGenericEffectorTransform &xml_instnace, ObjectNamespacePtr object_namespace);
	
		virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian);
	
		DampedGenericEffectorTransform(unsigned int task_dim, unsigned int resource_dim,	Float damping_constant = 0.5)
		{
			init(task_dim, resource_dim, damping_constant);
		}
	
		virtual void exec(const FloatVector &input, FloatVector &result) {
			result = m_InverseTaskJacobian * input;
		}


		void init(unsigned int task_dim, unsigned int resource_dim, Float damping_constant) {
			m_InverseTaskJacobian = FloatMatrix((int) resource_dim, (int) task_dim);
			m_DampingConstant = damping_constant;
		}	
	
		void setDampingConstant (Float damping_constant) {
			m_DampingConstant = damping_constant;
		}

		Float getDampingConstant () const {
			return m_DampingConstant;
		}

		protected:
			Float m_DampingConstant;
	};
	
	typedef boost::shared_ptr<DampedGenericEffectorTransform> DampedGenericEffectorTransformPtr;


	/**
		@brief Pseudo inverse based generic effector transform (damped, non-weighted)
	*/
	struct ThresholdGenericEffectorTransform : public EffectorTransform {
		ThresholdGenericEffectorTransform (const CBFSchema::ThresholdGenericEffectorTransform &xml_instnace, ObjectNamespacePtr object_namespace);

		virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian);

		ThresholdGenericEffectorTransform(unsigned int task_dim, unsigned int resource_dim,	Float threshold = 10)
		{
			init(task_dim, resource_dim, threshold);
		}

		virtual void exec(const FloatVector &input, FloatVector &result) {
			result = m_InverseTaskJacobian * input;
		}


		void init(unsigned int task_dim, unsigned int resource_dim, Float threshold) {
			m_InverseTaskJacobian = FloatMatrix((int) resource_dim, (int) task_dim);
			m_Threshold = threshold;
		}

		void setThreshold (Float threshold) {
			m_Threshold = threshold;
		}

		Float getThreshold () const {
			return m_Threshold;
		}

		protected:
			Float m_Threshold;
	};

	typedef boost::shared_ptr<ThresholdGenericEffectorTransform> ThresholdGenericEffectorTransformPtr;


	/**
		@brief Effector Transform based on padded Jacobian pseudo inverse
	*/
	struct PaddedEffectorTransform : public EffectorTransform {
		PaddedEffectorTransform (const CBFSchema::PaddedEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace);

		PaddedEffectorTransform(
			unsigned int task_dim, 
			unsigned int resource_dim, 
			FloatVector diagonal_elements
		) { 
			init(task_dim, resource_dim, diagonal_elements);
		}

		PaddedEffectorTransform(
			unsigned int task_dim, 
			unsigned int resource_dim, 
			float diagonal
		) { 
			FloatVector diagonal_elements = FloatVector::Constant(task_dim, diagonal);
			init(task_dim, resource_dim, diagonal_elements);
		}
	
		virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
			CBF_DEBUG("update padded");
			pseudo_inverse(task_jacobian, m_InverseTaskJacobian);

			m_PaddedTaskJacobian.block(0, 0, task_jacobian.rows(), task_jacobian.cols())
					= task_jacobian;
			CBF_DEBUG("padded jacobian: " << m_PaddedTaskJacobian);

			pseudo_inverse(m_PaddedTaskJacobian, m_PaddedInverseTaskJacobian);
		}
	
		virtual void exec(const FloatVector &input, FloatVector &result) {
			FloatVector res = m_PaddedInverseTaskJacobian * input;
			CBF_DEBUG("padded result: " << res);
			result = res.segment(0, m_InverseTaskJacobian.rows());
		}

		void init(unsigned int task_dim, unsigned int resource_dim, FloatVector diagonal) {
			CBF_DEBUG("Padded transform init");
			if (task_dim != (unsigned int)diagonal.size()) 
				CBF_THROW_RUNTIME_ERROR("dimension mismatch");

			m_PaddedTaskJacobian = FloatMatrix::Zero(task_dim, resource_dim + task_dim);

			for (unsigned int i = 0; i < task_dim; ++i) {
				m_PaddedTaskJacobian(i, resource_dim+i) = diagonal[i];
			}

			m_PaddedInverseTaskJacobian = FloatMatrix((int) resource_dim + task_dim, (int) task_dim);
			m_InverseTaskJacobian = FloatMatrix((int) resource_dim, (int) task_dim);
		}	


		protected:
			FloatMatrix m_PaddedTaskJacobian;
			FloatMatrix m_PaddedInverseTaskJacobian;
	};
	
	typedef boost::shared_ptr<PaddedEffectorTransform> PaddedEffectorTransformPtr;
	
	
} // namespace

#endif


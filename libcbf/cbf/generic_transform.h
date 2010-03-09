/* -*- mode: c-non-suck; -*- */

#ifndef CBF_GENERIC_TRANSFORM_H
#define CBF_GENERIC_TRANSFORM_H

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/utilities.h>
#include <cbf/plugin_decl_macros.h>


#include <boost/numeric/ublas/io.hpp>

CBF_PLUGIN_PREAMBLE(GenericEffectorTransform)
CBF_PLUGIN_PREAMBLE(DampedWeightedGenericEffectorTransform)

namespace CBF {

	/**
		@brief Pseudo inverse based generic effector transform (non-damped, non-weighted)
	*/
	struct GenericEffectorTransform : public EffectorTransform {
		CBF_PLUGIN_DECL_METHODS(GenericEffectorTransform)
	
		virtual void update();
	
		GenericEffectorTransform(SensorTransformPtr sensor_transform = SensorTransformPtr())
		{
			set_sensor_transform(sensor_transform);
		}
	
		virtual void exec(const FloatVector &input, FloatVector &result) {
			result = ublas::prod(m_InverseTaskJacobian, input);
		}
	
		virtual void inverse_task_jacobian(FloatMatrix &result) {
			result = m_InverseTaskJacobian;
		}
	
		virtual unsigned resource_dim() {
			return m_SensorTransform->resource_dim();
		}
	
		virtual unsigned int task_dim() {
			return m_SensorTransform->task_dim();
		}
	};
	
	
	class DampedWeightedGenericEffectorTransform;
	typedef boost::shared_ptr<DampedWeightedGenericEffectorTransform> DampedWeightedGenericEffectorTransformPtr;
	
	/**
		@brief Pseudo inverse based generic effector transform (damped, weighted)
	*/
	struct DampedWeightedGenericEffectorTransform : public EffectorTransform {
		CBF_PLUGIN_DECL_METHODS(DampedWeightedGenericEffectorTransform)

		protected:	
	 		FloatMatrix m_Weights;

		public:	
			virtual void update();
		
			DampedWeightedGenericEffectorTransform(SensorTransformPtr sensor_transform = SensorTransformPtr())
			{
				set_sensor_transform(sensor_transform);
			}
		
		
			virtual void exec(const FloatVector &input, FloatVector &result) {
				result = ublas::prod(m_InverseTaskJacobian, input);
			}
		
			virtual void inverse_task_jacobian(FloatMatrix &result) {
				result = m_InverseTaskJacobian;
			}
		
			virtual unsigned resource_dim() {
				return m_SensorTransform->resource_dim();
			}
		
			virtual unsigned int task_dim() {
				return m_SensorTransform->task_dim();
			}
	};
	
	typedef boost::shared_ptr<GenericEffectorTransform> GenericEffectorTransformPtr;
	
} // namespace

#endif


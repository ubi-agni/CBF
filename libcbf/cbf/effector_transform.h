#ifndef CONTROL_BASIS_EFFECTOR_TRANSFORM_HH
#define CONTROL_BASIS_EFFECTOR_TRANSFORM_HH

#include <cbf/resource.h>
#include <cbf/types.h>
#include <cbf/sensor_transform.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	
	class EffectorTransform;
	typedef boost::shared_ptr<EffectorTransform> EffectorTransformPtr;
	
	
	/**
		@brief The effector transform takes a gradient step in task space
		and maps it to the resource's configuration space by way of local
		pseudoinverse control..
	*/
	struct EffectorTransform {
		FloatVector m_ResourceValue;
	
		/**
			Sensor and Effector transforms always come in pairs. So before doing anything with
			this effector transform, be sure to set this member.
		*/
		SensorTransformPtr m_SensorTransform;
	
		EffectorTransform() {
	
		}

		/**
			A virtual destructor to make destruction of objects of derived types possible
			by deleting a pointer to the base class (this).
		*/	
		virtual ~EffectorTransform() {
	
		}
	
		virtual void set_sensor_transform(SensorTransformPtr sensor_transform) {
			m_SensorTransform = sensor_transform;
		}
	
		/**
			This method must be called before exec. Ideally right after the resource has 
			changed and the SensorTransfor has been read(). This method is meant for 
			EffectorTransforms to be able to do one-shot expensive computations whose 
			results will consequently be used by different methods, e.g. 
			the inverse task jacobian given that it depends on the current resource
			value / the current task jacobian (in the common case that it is not constant).
		*/
		virtual void update() = 0;

	
		/**
			This purely virtual method must be implemented by subclasses. 
			Its result is then passed on to the resource by the controller.
	
			This method must map the input gradient step onto a step in resource space.
			This step is then added lateron to the current resource values by the
			controller.
		*/
		virtual void exec(
			const FloatVector &input,
			FloatVector &result
		) = 0;
	
		/**
			A way to get to the current task jacobian. This is needed by the
			controller to construct the nullspace projector.
	
			May only be called after a call to exec() to update the internal
			matrices.
		*/
		virtual FloatMatrix &inverse_task_jacobian() { return m_InverseTaskJacobian; }
	
	
	
		/**
			Needs to be implemented in subclass to allow dimensionality checking when
			this is bound to a resource.
		*/
		virtual unsigned resource_dim() = 0;
	
		ResourcePtr resource() {
			return m_Resource;
		}
	
		virtual void set_resource(ResourcePtr r) {
			if (r->dim() != resource_dim()) throw std::runtime_error("[EffectorTransform]: Dimension mismatch!");
	
			m_Resource = r;
		}
	
		/**
			Needs to be implemented in subclass to allow dimensionality checking when
			this is bound to a resource.
		*/
		virtual unsigned int task_dim() = 0;
	
		protected:
			/** 
				An effector transform is bound to a resource
			*/
			ResourcePtr m_Resource;

			/**
				This should be calculated in the update() function. the inverse_task_jacobian() function
				should then return a reference to this to avoid unnessecary recomputations.
			*/
			FloatMatrix m_InverseTaskJacobian;
	};
} // namespace

#endif


#ifndef CBF_DIFFERENCE_SENSOR_TRANSFORM
#define CBF_DIFFERENCE_SENSOR_TRANSFORM

#include <cbf/sensor_transform.h>
#include <cbf/plugin_decl_macros.h>

#include <vector>
#include <cassert>

CBF_PLUGIN_PREAMBLE(DifferenceSensorTransform)

namespace CBF {

	/**
		@brief: A SensorTransform to calculate differences of two given SensorTransforms 
		operating on the same resources..
	*/
	struct DifferenceSensorTransform : public SensorTransform {
		CBF_PLUGIN_DECL_METHODS(DifferenceSensorTransform)

		DifferenceSensorTransform(std::vector<SensorTransformPtr> transforms = std::vector<SensorTransformPtr>()) {
			if (transforms.size() != 0)
				set_transforms(transforms);
		}

		void set_transforms(std::vector<SensorTransformPtr> &transforms) {
			if (transforms.size() != 2) 
				throw std::runtime_error("[DifferenceSensorTransform]: Number of transforms != 2");

			if (transforms[0]->resource_dim() != transforms[1]->resource_dim())
				throw std::runtime_error("[DifferenceSensorTransform]: Resource dimensions do not match");

			if (transforms[0]->task_dim() != transforms[1]->task_dim())
				throw std::runtime_error("[DifferenceSensorTransform]: Task dimensions do not match");

			m_Transforms = transforms;
		}

		void set_resource(ResourcePtr resource) {
			m_Transforms[0]->set_resource(resource);
			m_Transforms[1]->set_resource(resource);
			m_Resource = resource;
		}

		void update() {
			assert(m_Transforms.size() == 2);
			assert(m_Transforms[0]->task_dim() == m_Transforms[1]->task_dim());

			m_Transforms[0]->update();
			m_Transforms[1]->update();

			//! The jacobian is just the difference of the individual transforms
			m_TaskJacobian = m_Transforms[0]->task_jacobian() - m_Transforms[1]->task_jacobian();

			//! And the result is just the difference of the individual results.
			m_Result = m_Transforms[0]->result() - m_Transforms[1]->result();
		}

		// As both sensor transforms are required to have the same resource dimensionality, it does not matter
		// which one we return
		unsigned int resource_dim() { return m_Transforms[0]->resource_dim(); }

		// As both sensor transforms are required to have the same task dimensionality, it does not matter
		// which one we return
		unsigned int task_dim() { return m_Transforms[0]->task_dim(); }

		protected:
			std::vector<SensorTransformPtr> m_Transforms;
	};

	typedef boost::shared_ptr<DifferenceSensorTransform>
		DifferenceSensorTransformPtr;
	
} // namespace

#endif

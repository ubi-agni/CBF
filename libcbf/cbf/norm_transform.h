#ifndef CBF_NORM_TRANSFORM_HH
#define CBF_NORM_TRANSFORM_HH

#include <cbf/sensor_transform.h>
#include <cbf/plugin_decl_macros.h>
#include <cbf/exceptions.h>

CBF_PLUGIN_PREAMBLE(NormSensorTransform)

namespace CBF {

/**
	This class implements a SensorTransform that calculates the norm
	of the output of another given SensorTransform.
*/
struct NormSensorTransform : public SensorTransform {
	CBF_PLUGIN_DECL_METHODS(NormSensorTransform)
	/**
		The wrapped SensorTransform
	*/
	SensorTransformPtr m_Transform;

	NormSensorTransform(SensorTransformPtr transform) : 
		m_Transform(transform) {

	}

	virtual void update() {
		CBF_THROW_RUNTIME_ERROR("meeeh!!! implement me!!!");
	}

	virtual unsigned int task_dim() const { return 1u; }

	virtual unsigned int resource_dim() const { return m_Transform->resource_dim(); }
};


} // namespace

#endif

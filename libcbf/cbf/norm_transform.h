#ifndef CBF_NORM_TRANSFORM_HH
#define CBF_NORM_TRANSFORM_HH

#include <cbf/sensor_transform.h>

namespace CBF {

/**
	This class implements a SensorTransform that calculates the norm
	of the output of another given SensorTransform.
*/
struct NormSensorTransform : public SensorTransform {
	/**
		The wrapped SensorTransform
	*/
	SensorTransformPtr m_Transform;

};


} // namespace

#endif

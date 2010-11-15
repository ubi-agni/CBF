#ifndef CBF_CPPAD_ST_HH
#define CBF_CPPAD_ST_HH

#include <cbf/sensor_transform.h>
#include <cbf/types.h>

#include <cppad/CppAD.h>

namespace CBF {

using CppAD::AD;

struct CppADSensorTransform : public SensorTransform {
	CppAD<Float> m_Func;

	CppADSensorTransform(const CppAD::ADFun<Float> &fun) {

	}
};

} // namespace

#endif

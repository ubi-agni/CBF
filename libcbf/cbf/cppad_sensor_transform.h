#ifndef CBF_CPPAD_ST_HH
#define CBF_CPPAD_ST_HH

#include <cbf/sensor_transform.h>
#include <cbf/types.h>

#include <cppad/CppAD.h>

namespace CBF {

using CppAD::AD;

struct CppADSensorTransform : public SensorTransform {
	CppAD::ADFun<Float> m_Func;

	CppADSensorTransform(const CppAD::ADFun<Float> &fun) {

	}

	virtual void update(const FloatVector &resource_value) {
		std::vector<double> TaskJacobian = m_Func.Jacobian(std::vector<double>(1));
	}
};

} // namespace

#endif

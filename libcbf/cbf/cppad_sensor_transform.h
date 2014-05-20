#ifndef CBF_CPPAD_ST_HH
#define CBF_CPPAD_ST_HH

// for some reason, the include order is important
#include <cppad/example/cppad_eigen.hpp>

#include <cbf/sensor_transform.h>
#include <cbf/types.h>

#include <boost/shared_ptr.hpp>

namespace CBF {

typedef Eigen::Matrix< CppAD::AD<Float>, Eigen::Dynamic, 1> ADFloatVector;

/**
	@brief A SensorTransform that is parametrized with an AppAD::ADFun<Float> which enables automatic calculation of the task jacobian (CppAD is an automatic differentiation package)
*/
struct CppADSensorTransform : public SensorTransform {
	CppAD::ADFun<Float> m_Func;

	CppADSensorTransform(const CppAD::ADFun<Float> &fun, 
								unsigned int task_dim, 
								unsigned int resource_dim) {
		assert(fun.Domain() == resource_dim);
		assert(fun.Range()  == task_dim);
		m_Func = fun;
		m_Result = FloatVector(task_dim);
		m_TaskJacobian = FloatMatrix((int)task_dim, (int)resource_dim);
	}

	virtual void update(const FloatVector &resource_value) {
		m_Result = m_Func.Forward(0, resource_value);
		// this creates a task_dim * resource_dim vector
		m_TaskJacobian = m_Func.Jacobian(resource_value);
		// correctly reshape matrix:
		m_TaskJacobian.conservativeResize(m_Func.Range(), m_Func.Domain());
	}
};

typedef boost::shared_ptr<CppADSensorTransform> CppADSensorTransformPtr;

} // namespace

#endif

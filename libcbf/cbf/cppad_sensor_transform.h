#ifndef CBF_CPPAD_ST_HH
#define CBF_CPPAD_ST_HH

#include <cbf/sensor_transform.h>
#include <cbf/types.h>

#include <cppad/CppAD.h>
#include <cppad/check_simple_vector.hpp> 

#include <boost/shared_ptr.hpp>

namespace CBF {

using CppAD::AD;


/**
	@brief A SensorTransform that is parametrized with an AppAD::ADFun<Float> which enables automatic calculation of the task jacobian (CppAD is an automatic differentiation package)
*/
struct CppADSensorTransform : public SensorTransform {
	CppAD::ADFun<Float> m_Func;

	CppADSensorTransform(const CppAD::ADFun<Float> &fun, unsigned int task_dim, unsigned int resource_dim) {
		m_Func = fun;
		CppAD::CheckSimpleVector<Float, FloatVector>();
		m_Result = FloatVector(task_dim);
		m_TaskJacobian = FloatMatrix(task_dim, resource_dim);
		m_Tmp = FloatVector(task_dim * resource_dim);
	}

	virtual void update(const FloatVector &resource_value) {
		FloatVector m_Tmp = m_Func.Jacobian<FloatVector>(resource_value);

		unsigned int cols = m_TaskJacobian.size2();
		for (unsigned int row = 0, mrow = m_TaskJacobian.size1(); row < mrow; ++row) {
			for (unsigned int col = 0, mcol = m_TaskJacobian.size2(); col < mcol; ++col) {
				m_TaskJacobian(row, col) = m_Tmp[cols * row + col];
			}
		}
		m_Result = m_Func.Forward<FloatVector>(0, resource_value);
	}

	protected:
		FloatVector m_Tmp;
};

typedef boost::shared_ptr<CppADSensorTransform> CppADSensorTransformPtr;

} // namespace

#endif

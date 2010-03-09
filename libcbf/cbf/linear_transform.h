#ifndef CBF_LINEAR_TRANSFORM_HH
#define CBF_LINEAR_TRANSFORM_HH

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>

#include <cbf/plugin_decl_macros.h>
#include <cbf/utilities.h>

CBF_PLUGIN_PREAMBLE(LinearSensorTransform)

namespace CBF {

class LinearSensorTransform;
typedef boost::shared_ptr<LinearSensorTransform> LinearSensorTransformPtr;

/**
	@brief A linear sensor transform
*/
struct LinearSensorTransform : public SensorTransform {
	CBF_PLUGIN_DECL_METHODS(LinearSensorTransform)

	FloatMatrix m_Matrix;

	void update() {
		m_Result = ublas::prod(m_Matrix, m_Resource->get());
	}

	LinearSensorTransform() :
		m_Matrix(ublas::identity_matrix<Float>(1))
	{

	}

	LinearSensorTransform(FloatMatrix &m) :
		m_Matrix(m)
	{

	}

	/**
		Needs to be implemented in subclass to allow dimensionality checking when
		this is bound to a resource.
	*/
	virtual unsigned int resource_dim() {
		return m_Matrix.size2();
	}

	/**
		Needs to be implemented in subclass to allow dimensionality checking when
		this is bound to a resource.
	*/
	virtual unsigned int task_dim() {
		return m_Matrix.size1();
	}
};


} // namespace

#endif

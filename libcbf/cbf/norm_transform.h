#ifndef CBF_NORM_TRANSFORM_HH
#define CBF_NORM_TRANSFORM_HH

#include <cbf/sensor_transform.h>
#include <cbf/plugin_decl_macros.h>
#include <cbf/exceptions.h>
#include <cbf/types.h>
#include <cbf/config.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

CBF_PLUGIN_PREAMBLE(NormSensorTransform)

namespace CBF {

namespace ublas = boost::numeric::ublas;

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

		/// The norm is a 1D number
		m_Result.resize(1);

		m_TaskJacobian.resize(1,transform->resource_dim());
	}

	virtual void update() {
		m_Transform->update();
		m_Result[0] = ublas::norm_2(m_Transform->result());

		FloatVector res2 = ublas::prod(ublas::trans(m_Transform->result()), m_Transform->task_jacobian());

		for (unsigned int i = 0; i < res2.size(); ++i) {
			m_TaskJacobian(0,i) = res2(i);
		}
	}

	virtual unsigned int task_dim() const { return 1u; }

	virtual unsigned int resource_dim() const { return m_Transform->resource_dim(); }

	virtual void set_resource(ResourcePtr res) {
		m_Transform->set_resource(res);
		SensorTransform::set_resource(res);
	}
};


} // namespace

#endif

#ifndef CBF_NORM_TRANSFORM_HH
#define CBF_NORM_TRANSFORM_HH

#include <cbf/sensor_transform.h>
#include <cbf/exceptions.h>
#include <cbf/types.h>
#include <cbf/config.h>
#include <cbf/debug_macros.h>
#include <cbf/namespace.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace CBFSchema { class NormSensorTransform; }

namespace CBF {

namespace ublas = boost::numeric::ublas;

/**
	This class implements a SensorTransform that calculates the norm
	of the output of another given SensorTransform.
*/
struct NormSensorTransform : public SensorTransform {
	NormSensorTransform(const CBFSchema::NormSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

	/**
		The wrapped SensorTransform
	*/
	SensorTransformPtr m_Transform;

	NormSensorTransform(SensorTransformPtr operand) 
	{
		init(operand);
	}

	virtual void init(SensorTransformPtr operand) {
		m_Transform = operand;
		m_Result.resize(1);
		m_TaskJacobian.resize(1, operand->resource_dim());
	}

	virtual void update(const FloatVector &resource_value) {
		m_Transform->update(resource_value);
		m_Result[0] = ublas::norm_2(m_Transform->result());

		FloatVector res2 = 
			(1.0/m_Result[0]) 
			* ublas::prod(
				ublas::trans(m_Transform->result()), 
				m_Transform->task_jacobian()
			);

		for (unsigned int i = 0; i < res2.size(); ++i) {
			m_TaskJacobian(0,i) = res2(i);
		}

		CBF_DEBUG(m_TaskJacobian);
	}

	/**
		@brief The norm gives a single real number
	*/
	virtual unsigned int task_dim() const { return 1u; }

	/**
		@brief The resource dimensionality is the same as of the operand
	*/
	virtual unsigned int resource_dim() const { return m_Transform->resource_dim(); }
};


} // namespace

#endif

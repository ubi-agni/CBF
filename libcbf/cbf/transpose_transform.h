#ifndef CBF_TRANSPOSE_TRANSFORM_HH
#define CBF_TRANSPOSE_TRANSFORM_HH

#include <cbf/effector_transform.h>

namespace CBFSchema { class TransposeEffectorTransform; }

namespace CBF {

struct TransposeEffectorTransform : public EffectorTransform {
	TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance);

	virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
		m_InverseTaskJacobian = ublas::trans(task_jacobian);
	}

	virtual unsigned int task_dim() const { return m_SensorTransform->task_dim(); }
	virtual unsigned int resource_dim() const { return m_SensorTransform->resource_dim(); }

	virtual void exec(const FloatVector &input, FloatVector &result) {
		result = ublas::prod(m_InverseTaskJacobian, input);
	}
};

} // namespace

#endif

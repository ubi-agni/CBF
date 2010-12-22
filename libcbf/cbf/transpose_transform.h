#ifndef CBF_TRANSPOSE_TRANSFORM_HH
#define CBF_TRANSPOSE_TRANSFORM_HH

#include <cbf/effector_transform.h>

namespace CBFSchema { class TransposeEffectorTransform; }

namespace CBF {

struct TransposeEffectorTransform : public EffectorTransform {
	TransposeEffectorTransform(unsigned int task_dim, unsigned int resource_dim) {
		m_InverseTaskJacobian = FloatMatrix(resource_dim, task_dim);
	}

	TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance);

	virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
		m_InverseTaskJacobian = ublas::trans(task_jacobian);
	}

	virtual void exec(const FloatVector &input, FloatVector &result) {
		result = ublas::prod(m_InverseTaskJacobian, input);
	}
};

} // namespace

#endif

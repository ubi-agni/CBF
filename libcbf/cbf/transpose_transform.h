#ifndef CBF_TRANSPOSE_TRANSFORM_HH
#define CBF_TRANSPOSE_TRANSFORM_HH

#include <cbf/effector_transform.h>
#include <cbf/namespace.h>

namespace CBFSchema { class TransposeEffectorTransform; }

namespace CBF {

struct TransposeEffectorTransform : public EffectorTransform {
	TransposeEffectorTransform(unsigned int task_dim, unsigned int resource_dim) {
		m_InverseTaskJacobian = FloatMatrix((int) resource_dim, (int) task_dim);
	}

	TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace);

	virtual void update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
		m_InverseTaskJacobian = task_jacobian.transpose();
	}

	virtual void exec(const FloatVector &input, FloatVector &result) {
		result = m_InverseTaskJacobian * input;
	}
};

} // namespace

#endif

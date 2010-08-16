#ifndef CBF_TRANSPOSE_TRANSFORM_HH
#define CBF_TRANSPOSE_TRANSFORM_HH

#include <cbf/effector_transform.h>
#include <cbf/plugin_decl_macros.h>

CBF_PLUGIN_PREAMBLE(TransposeEffectorTransform)

namespace CBF {

struct TransposeEffectorTransform : public EffectorTransform {
	CBF_PLUGIN_DECL_METHODS(TransposeEffectorTransform)

	virtual void update() {
		m_InverseTaskJacobian = ublas::trans(m_SensorTransform->task_jacobian());
	}

	virtual unsigned int task_dim() const { return m_SensorTransform->task_dim(); }
	virtual unsigned int resource_dim() const { return m_SensorTransform->resource_dim(); }

	virtual void exec(const FloatVector &input, FloatVector &result) {
		result = ublas::prod(m_InverseTaskJacobian, input);
	}
};

} // namespace

#endif

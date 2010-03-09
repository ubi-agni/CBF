#include <cbf/generic_transform.h>
#include <cbf/plugin_macros.h>

namespace CBF {

void GenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}


void DampedWeightedGenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}


#ifdef CBF_HAVE_XSD
	GenericEffectorTransform::GenericEffectorTransform(const GenericEffectorTransformType &xml_instance)
	{
	
	}
	
	DampedWeightedGenericEffectorTransform::DampedWeightedGenericEffectorTransform(const DampedWeightedGenericEffectorTransformType &xml_instance)
	{
	
	}
#endif

CBF_PLUGIN_CLASS(GenericEffectorTransform, EffectorTransform)
CBF_PLUGIN_CLASS(DampedWeightedGenericEffectorTransform, EffectorTransform)


} // namespace


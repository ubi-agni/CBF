#include <cbf/identity_transform.h>
#include <cbf/plugin_macros.h>

namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		IdentityEffectorTransform::IdentityEffectorTransform(const IdentityEffectorTransformType &xml_instance) :
			m_Dim(xml_instance.Dimension())
		{
			m_InverseTaskJacobian = boost::numeric::ublas::identity_matrix<Float>(xml_instance.Dimension());

			std::cout << "[IdentityEffectorTransform(const IdentityEffectorTransformType &xml_instance)]: yay!!!" << std::endl;
		}
		
		IdentitySensorTransform::IdentitySensorTransform(const IdentitySensorTransformType &xml_instance) :
			m_Dim(xml_instance.Dimension())
		{
			std::cout << "[IdentitySensorTransform(const IdentitySensorTransformType &xml_instance)]: yay!!!" << std::endl;
			m_TaskJacobian = boost::numeric::ublas::identity_matrix<Float>(m_Dim,m_Dim);
		}
	#endif
	
	CBF_PLUGIN_CLASS(IdentitySensorTransform, SensorTransform)
} // namespace


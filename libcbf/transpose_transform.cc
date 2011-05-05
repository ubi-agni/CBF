#include <cbf/transpose_transform.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	TransposeEffectorTransform::TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace) 
	{
		m_InverseTaskJacobian = FloatMatrix(
				xml_instance.ResourceDimension(), xml_instance.TaskDimension()
		);
	}

	static XMLDerivedFactory<TransposeEffectorTransform, CBFSchema::TransposeEffectorTransform> x;

#endif

} // namespace


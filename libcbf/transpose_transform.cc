#include <cbf/transpose_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	TransposeEffectorTransform::TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace) 
	{
		m_InverseTaskJacobian = FloatMatrix(
				(int) xml_instance.ResourceDimension(),
				(int) xml_instance.TaskDimension()
		);
	}

	static XMLDerivedFactory<TransposeEffectorTransform, CBFSchema::TransposeEffectorTransform> x;

#endif

} // namespace


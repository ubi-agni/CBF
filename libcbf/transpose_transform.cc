#include <cbf/transpose_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	TransposeEffectorTransform::TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance) :
		EffectorTransform(xml_instance.TaskDimension(), xml_instance.ResourceDimension())
	{

	}

	static XMLDerivedFactory<TransposeEffectorTransform, CBFSchema::TransposeEffectorTransform> x;

#endif

} // namespace


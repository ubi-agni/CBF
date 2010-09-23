#include <cbf/transpose_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	TransposeEffectorTransform::TransposeEffectorTransform(const CBFSchema::TransposeEffectorTransform &xml_instance) {

	}

	static XMLDerivedFactory<TransposeEffectorTransform, CBFSchema::TransposeEffectorTransform, EffectorTransform, CBFSchema::EffectorTransform> x;

#endif

} // namespace


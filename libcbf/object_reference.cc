#include <cbf/object_reference.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	ObjectReference::ObjectReference(
		const CBFSchema::ObjectReference &xml_instance, 
		ObjectNamespacePtr object_namespace
	) {
		init(xml_instance.ReferenceName());
	}

	static XMLDerivedFactory<ObjectReference, CBFSchema::ObjectReference> x;


#endif

} // namespace
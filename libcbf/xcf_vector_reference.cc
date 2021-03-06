#include <cbf/xcf_vector_reference.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD 
	XCFVectorReference::XCFVectorReference(const CBFSchema::XCFVectorReference &xml_instance, ObjectNamespacePtr object_namespace) :
		m_XCFServer(XCF::Server::create(xml_instance.ServerName())), 
		m_Dim(xml_instance.Dimension())
	{ 	
		init();
	}

	static XMLDerivedFactory<XCFVectorReference, CBFSchema::XCFVectorReference> x;
#endif
} // namespace



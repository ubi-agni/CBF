#include <cbf/xcf_vector_reference.h>

namespace CBF {

#ifdef CBF_HAVE_XSD 
XCFVectorReference::XCFVectorReference(const CBFSchema::XCFVectorReference &xml_instance) :
		m_XCFServer(XCF::Server::create(xml_instance.Name())), 
		m_Dim(xml_instance.Dimension())
	{ 	
		init();
	}
#endif

} // namespace



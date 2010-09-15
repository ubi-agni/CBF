#include <cbf/xcf_vector_reference.h>
#include <cbf/plugin_impl_macros.h>

namespace CBF {

#ifdef CBF_HAVE_XSD 
XCFVectorReference::XCFVectorReference(const XCFVectorReferenceType &xml_instance) :
		m_XCFServer(XCF::Server::create(xml_instance.Name())), 
		m_Dim(xml_instance.Dimension())
	{ 	
		init();
	}
#endif

CBF_PLUGIN_CLASS(XCFVectorReference, Reference);

} // namespace



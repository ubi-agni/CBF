#include <cbf/xcf_vector_reference.h>
#include <cbf/plugin_impl_macros.h>

namespace CBF {

#ifdef CBF_HAVE_XSD 
XCFVectorReference::XCFVectorReference(const XCFVectorReferenceType &xml_instance) :
		m_XCFServer(XCF::Server::create(xml_instance.Name())), 
		m_Dim(xml_instance.Dimension()),
		m_TempReference(m_Dim)
	{ 	
		m_References.resize(1);
		m_References[0].resize(m_Dim);

		boost::function<void (std::string&, std::string&) > f =
			boost::bind(
				boost::mem_fn(&XCFVectorReference::set_reference_from_xcf), 
				this, 
				_1);

		m_XCFServer->registerMethod
			(std::string("set_reference"), f);

		m_XCFServer->run(true);
	}
#endif

CBF_PLUGIN_CLASS(XCFVectorReference, Reference);

} // namespace



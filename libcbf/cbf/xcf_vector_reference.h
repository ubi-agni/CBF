#ifndef CBF_XCF_VECTOR_REFERENCE_HH
#define CBF_XCF_VECTOR_REFERENCE_HH

#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/exceptions.h>

#include <xcf/ServerComponent.hpp>
#include <IceUtil/Monitor.h> 
#include <IceUtil/RecMutex.h>
#include <cbf/schemas.hxx>
#include <cbf/utilities.h>
#include <cbf/plugin_decl_macros.h>

#include <boost/bind.hpp>

#include <string>
#include <memory>

CBF_PLUGIN_PREAMBLE(XCFVectorReference)

namespace CBF {

struct XCFVectorReference : public Reference {
	CBF_PLUGIN_DECL_METHODS(XCFVectorReference)

	XCF::ServerPtr m_XCFServer;
	unsigned int m_Dim;

	/** 
		This member is used as a protected resource to transfer the
		reference values from the XCF server thread to the thread of
		the main 
	*/
	FloatVector m_TempReference;

	IceUtil::Monitor<IceUtil::RecMutex> m_ReferenceMonitor;

	XCFVectorReference
		(const std::string &server_name, unsigned int dim = 1) 
		: 
		m_XCFServer(XCF::Server::create(server_name)), 
		m_Dim(dim),
		m_TempReference(dim)
	{ 	
		m_References.resize(0);

		boost::function<void (std::string&, std::string&) > f =
			boost::bind(
				boost::mem_fn(&XCFVectorReference::set_reference_from_xcf), 
				this, 
				_1);

		m_XCFServer->registerMethod
			(std::string("set_reference"), f);

		m_XCFServer->run(true);
	}

	/** update is a noop in this case as updates are handled asynchronously */
	virtual void update()  {
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		m_References.resize(1);
		m_References[0] = m_TempReference;
	}

	virtual unsigned int dim() { return m_Dim; }

	virtual void set_reference_from_xcf(std::string &xml_in) {
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		std::auto_ptr<VectorType> v = Vector(xml_in);
		m_TempReference = create_vector(*v);
		if (m_TempReference.size() != dim())
			CBF_THROW_RUNTIME_ERROR("dimensions of xml vector not matching the dimension of this reference");
	}
};

typedef boost::shared_ptr<XCFVectorReference> XCFVectorReferencePtr;

} // namespace

#endif

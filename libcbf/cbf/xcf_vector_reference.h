#ifndef CBF_XCF_VECTOR_REFERENCE_HH
#define CBF_XCF_VECTOR_REFERENCE_HH

#include <cbf/reference.h>
#include <cbf/types.h>

#include <xcf/ServerComponent.hpp>
#include <IceUtil/Monitor.h> 
#include <IceUtil/RecMutex.h>
#include <cbf/schemas.hxx>
#include <cbf/utilities.h>

#include <boost/bind.hpp>

#include <string>
#include <memory>

namespace CBF {

struct XCFVectorReference : public Reference {
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
		(std::string server_name, unsigned int dim = 1) 
		: 
		m_XCFServer(XCF::Server::create(server_name)), m_Dim(dim) 
	{ 	
		m_References.resize(1);
		boost::function<void (std::string&, std::string&) > f =
			boost::bind(
				boost::mem_fn
					(&XCFVectorReference::set_reference_from_xcf), 
				this, 
				_1);

		m_XCFServer->registerMethod
			(std::string("set_reference"), f);

		m_XCFServer->run(true);
	}

	/** update is a noop in this case as updates are handled asynchronously */
	virtual void update()  {
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		m_References[0] = m_TempReference;
	}

	virtual unsigned int dim() { return m_Dim; }

	virtual void set_reference_from_xcf(std::string &xml_in) {
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		std::auto_ptr<VectorType> v = Vector(xml_in);
		m_TempReference = create_vector(*v.get());
	}
};

typedef boost::shared_ptr<XCFVectorReference> XCFVectorReferencePtr;

} // namespace

#endif

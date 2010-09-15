#ifndef CBF_XCF_VECTOR_REFERENCE_HH
#define CBF_XCF_VECTOR_REFERENCE_HH

#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/exceptions.h>
#include <cbf/debug_macros.h>

#include <xcf/ServerComponent.hpp>
#include <IceUtil/Monitor.h> 
#include <IceUtil/RecMutex.h>
#include <cbf/schemas.hxx>
#include <cbf/utilities.h>
#include <cbf/plugin_decl_macros.h>
#include <cbf/schemas.hxx>

#include <boost/bind.hpp>

#include <string>
#include <memory>

CBF_PLUGIN_PREAMBLE(XCFVectorReference)

namespace CBF {

/**
	@brief This reference exposes the set_reference method via 
	an XCF server. This can be used to set the reference
	of a controller over the network..

	The set_reference method expects an xml document matching the 
	VectorType schema [or one of its derived types]

	See the test examples test/cbf_test_xcf_vector_resource_client.cc
	to see how to remotely call this method
*/
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

	/**
		Creates an XCFReference registering the server named server_name
		and exposing the method set_reference. The constructor 
		also expects a dimension argument. Later RMIs are checked
		for matching dimensionality.
	*/
	XCFVectorReference
		(const std::string &server_name, unsigned int dim = 1) 
		: 
		m_XCFServer(XCF::Server::create(server_name)), 
		m_Dim(dim)
	{ 	
		init();
	}

	void init() {
		boost::function<void (std::string&, std::string&) > f;

		f = boost::bind(
			boost::mem_fn(&XCFVectorReference::set_reference_from_xcf), 
			this,
			_1,
			_2
		);

		m_XCFServer->registerMethod
			(std::string("set_reference"), f);

		f = boost::bind(
			boost::mem_fn(&XCFVectorReference::get_dimension_from_xcf), 
			this,
			_1,
			_2
		);

		m_XCFServer->registerMethod
			(std::string("get_dimension"), f);

		m_XCFServer->run(true);
	}

	/** update is a noop in this case as updates are handled asynchronously */
	virtual void update()  {
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		if (m_TempReference.size() == m_Dim) {
			m_References.resize(1);
			m_References[0] = m_TempReference;
		}
	}

	virtual unsigned int dim() { return m_Dim; }

	virtual void get_dimension_from_xcf(std::string &xml_in, std::string &xml_out) {
		std::stringstream vector_string;

		vector_string << "[1](" << dim() << ")";

		BoostVectorType v(vector_string.str());

		std::ostringstream s;
		Vector (s, v);

		CBF_DEBUG("dimension xcf" << s.str())

		xml_out = s.str();
	}

	/**
		This method is exposed to XCF as "set_reference"
	*/
	virtual void set_reference_from_xcf(std::string &xml_in, std::string &xml_out) {
		CBF_DEBUG("in")
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		CBF_DEBUG("locked")

		CBF_DEBUG("doc: " << xml_in)
		std::istringstream s(xml_in);
		std::auto_ptr<VectorType> v = Vector(s, xml_schema::flags::dont_validate);
		CBF_DEBUG("create vector")
		m_TempReference = create_vector(*v);

		CBF_DEBUG("vector created")
		if (m_TempReference.size() != dim()) {
			CBF_DEBUG("meeeh!!!")
			CBF_THROW_RUNTIME_ERROR("Dimensions of xml vector not matching the dimension of this reference");
		}
		CBF_DEBUG("out")
	}
};

typedef boost::shared_ptr<XCFVectorReference> XCFVectorReferencePtr;

} // namespace

#endif

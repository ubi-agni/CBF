#ifndef CBF_XCF_MEMORY_REFERENCE_HH
#define CBF_XCF_MEMORY_REFERENCE_HH

#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/exceptions.h>
#include <cbf/debug_macros.h>
#include <cbf/schemas.hxx>
//#include <cbf/utilities.h>

#include <IceUtil/Monitor.h> 
#include <IceUtil/RecMutex.h>

#include <string>
#include <memory>

#include <Memory/Interface.hpp>
#include <xmltio/Location.hpp>
#include <xmltio/XPath.hpp>


namespace CBFSchema { class XCFMemoryReference; }

namespace CBF {



/**
	@brief This reference exposes the set_reference method via 
	XCFMemory. This can be used to set the reference of a controller..

	The set_reference method expects an xml document matching the 
	VectorType schema [or one of its derived types]
*/
struct XCFMemoryReference : public Reference {

	XCFMemoryReference(const CBFSchema::XCFMemoryReference &xml_instance);

	/**
		Creates an XCFMemoryReference connecting to the Memory named URI
		and adds a subscription for the resource named resource_name.
		The constructor also expects a dimension argument. Later RMIs
		are checked for matching dimensionality.
	*/
	XCFMemoryReference(const std::string &uri, const std::string &referenceName, unsigned int dim = 1);

	/** update is a noop in this case as updates are handled asynchronously */
	virtual void update();

	virtual unsigned int dim() { return m_Dim; }

	virtual void set_reference(const memory::interface::Event &event);

	protected:

	/** 
		This member is used as a protected resource to transfer the
		reference values from the XCFMemory thread to the thread of
		the main
	*/
	FloatVector m_TempReference;

	IceUtil::Monitor<IceUtil::RecMutex> m_ReferenceMonitor;
	
	/**
		@brief: The pointer to the XCFMemory server.
	*/
	memory::interface::MemoryInterface::pointer m_MemoryPtr;

	std::string m_ReferenceName;
	unsigned int m_Dim;

	void init();

	/**
		@brief: Returns the string that identifies the XML-document which is listened for.
	*/
	const char* XPathString(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryReference/ReferenceName['" << m_ReferenceName << "']";
		return xPathStream.str().c_str();
	}
};

typedef boost::shared_ptr<XCFMemoryReference> XCFMemoryReferencePtr;

} // namespace

#endif

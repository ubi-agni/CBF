/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2010 Viktor Richter
*/

#ifndef CBF_XCF_MEMORY_REFERENCE_HH
#define CBF_XCF_MEMORY_REFERENCE_HH

#include <cbf/config.h>

#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/exceptions.h>
#include <cbf/debug_macros.h>


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

	The Reference listens for an XCFMemoryReferenceVector with the specified
	ReferenceName on the MemoryServer referenced by the URL
. 
*/
struct XCFMemoryReference : public Reference {

	/**
		@brief Creates an XCFMemoryReference from an xml-instance.

		@param xml_instance An instance of a CBFSchema::XCFMemoryReference.
	*/
	XCFMemoryReference(const CBFSchema::XCFMemoryReference &xml_instance);

	/**
		@brief Creates an XCFMemoryReference connecting to the Memory named URI
		and adds a subscription for the resource named reference_name.
		The constructor also expects a dimension argument. Later RMIs
		are checked for matching dimensionality.

		@param uri The URI of the XCFMemory server that is to listen to.
		@param reference_name The name of the XCFMemoryReference.
		@param dim The dimesion of the reference.
	*/
	XCFMemoryReference(const std::string &uri, const std::string &reference_name, unsigned int dim = 1);

	/** 
		@brief update is a noop in this case as updates are handled asynchronously 
	*/
	virtual void update();

	/**
		@brief Returns the dimension of the reference.
	*/
	virtual unsigned int dim() { return m_Dim; }

	/**
		@brief Sets the reference. Expects a memory::interface::Event, that holds an
		XCFMemoryReferenceVector-document.
	*/
	virtual void set_reference(const memory::interface::Event &event);

	protected:

	/** 
		@brief This member is used as a protected resource to transfer the
		reference values from the XCFMemory thread to the thread of
		the main.
	*/
	FloatVector m_TempReference;

	/**
		@brief The mutex-lock that is used for the thread syncronization.
	*/
	IceUtil::Monitor<IceUtil::RecMutex> m_ReferenceMonitor;
	
	/**
		@brief: The pointer to the XCFMemory server.
	*/
	memory::interface::MemoryInterface::pointer m_MemoryInterface;

	/**
		@brief The name of the XCFMemoryReference.
	*/
	std::string m_ReferenceName;

	/**
		@brief The dimension of the Reference
	*/
	unsigned int m_Dim;

	/**
		@brief Utility function for the constructor.
	*/
	void init();

	/**
		@brief Returns the string that identifies the XML-document which is listened for.
	*/
	const std::string XPathString(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryReferenceVector[ReferenceName='" << m_ReferenceName << "']";
		return xPathStream.str();
	}
};

/**
	@brief A shared pointer to a XCFMemoryReference.
*/
typedef boost::shared_ptr<XCFMemoryReference> XCFMemoryReferencePtr;

} // namespace

#endif

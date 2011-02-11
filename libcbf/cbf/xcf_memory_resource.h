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


    Copyright 2011 Viktor Richter
*/

#ifndef CBF_XCF_MEMORY_RESOURCE_HH
#define CBF_XCF_MEMORY_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/resource.h>
#include <cbf/xml_factory.h>

#include <Memory/Condition.hpp>
#include <Memory/Subscription.hpp>
#include <Memory/Interface.hpp>
#include <Memory/Event.hpp>

#include <IceUtil/Monitor.h> 
#include <IceUtil/RecMutex.h>

#include <xmltio/Document.hpp>

#include <boost/bind.hpp>

namespace CBF {


/**
	@brief A resource that wraps another resource ande sends/retrieves values over an 
	ActiveMemory database
*/
struct XCFMemoryResource : public Resource {

	/**
		@brief Creates an XCFMemoryResource from an xml-instance.

		@param xml_instance An instance of a CBFSchema::XCFMemoryResource.
	*/	
	XCFMemoryResource(const CBFSchema::XCFMemoryResource &xml_instance);	

	/**
		@brief Creates an XCFMemoryResource connecting to the Memory named URI
		and adds a subscription for the resource named resource_name.

		@param resource The resource that will be wraped.
		@param uri The URI of the XCFMemory server that is to listen to.
		@param resource_name The name of the XCFMemoryReference.
	*/
	XCFMemoryResource(ResourcePtr resource , const std::string &uri, const std::string &resource_name);

	/**
		@brief Calls update on the wraped Resource then publishes the current
		numeric representation of the resource as a XCFMemoryResourceState on
		the Memory.
	*/
	virtual void update();

	/**
		@brief Returns the numeric representation of the wrapped resource.
	*/
	virtual const FloatVector &get(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ResourceMonitor);
		return m_Resource -> get();
	}

	/**
		@brief Calls the add() function of the wrapped resource. This function will
		be called by the triggered_action() function to add the new vector from the 
		memory to the resource.
	*/
	virtual void add(const FloatVector &arg){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ResourceMonitor);
		m_Resource -> add(arg);
	}

	/**
		@brief Returns the dimensoin of the wrapped resource.
	*/
	virtual unsigned int dim(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ResourceMonitor);
		return m_Resource -> dim();
	}

	protected:

		/**
			@brief Utility function for the constructor.
		*/
		void init();


		/**
			@brief This function will be called by the active_memory. It reads the vector from
			the event and calls the add() function of the wrapped resource.
		*/
		void triggered_action(const memory::interface::Event &event);


		/**
			@brief The type of events we listen to.
		*/
		const static int eventType = memory::interface::Event::INSERT
						| memory::interface::Event::REPLACE;


		/**
			@brief Returns the xPath for the condition of the subscription at the memory.
		*/
		const std::string memorySubscriptionXPath(){
			std::stringstream xPathStream;
			xPathStream << "/p1:XCFMemoryResourceAdd[ResourceName='" << m_ResourceName << "']";
			return xPathStream.str();
		}


		/**
			@brief Returns the xPath for the Location of the vector in the
			XCFMemoryResourceState-document.
		*/
		inline std::string documentVectorLocationXPath(){
			return "/p1:XCFMemoryResourceState/Vector/String";
		}


		/**
			@brief The mutex-lock that is used for the thread syncronization.
		*/
		IceUtil::Monitor<IceUtil::RecMutex> m_ResourceMonitor;


		/**
			@brief The name of the XCFMemoryResource.
		*/
		std::string m_ResourceName;


		/**
			@brief The pointer to the XCFMemory server.
		*/
		memory::interface::MemoryInterface::pointer m_MemoryInterface;


		/**
			@brief: The Resource, which is to wrap.
		*/
		ResourcePtr m_Resource;


		/**
			@brief: Points to the 'Vector' element of 
			the inserted XCFMemoryResourceState document.
		*/
		boost::shared_ptr<xmltio::Location> m_StateLocationPtr;
};

/**
	@brief A shared pointer to a XCFMemoryResource.
*/
typedef boost::shared_ptr<XCFMemoryResource> XCFMemoryResourcePtr;


} // namespace

#endif

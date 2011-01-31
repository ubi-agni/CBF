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

#include <cbf/xcf_memory_resource.h>
#include <cbf/utilities.h>
#include <xmltio/XPath.hpp>

namespace CBF {

	namespace mi = memory::interface;

	#ifdef CBF_HAVE_XSD
		XCFMemoryResource::XCFMemoryResource(const CBFSchema::XCFMemoryResource &xml_instance)
			:
			m_ResourceName(xml_instance.ResourceName()),
			m_MemoryInterface(mi::MemoryInterface::getInstance(xml_instance.URI())),
			m_Resource(XMLObjectFactory::instance() -> create<Resource>(xml_instance.Resource1()))
		{
			init();
		}

		static XMLDerivedFactory<XCFMemoryResource, CBFSchema::XCFMemoryResource> x;
	#endif

	XCFMemoryResource::XCFMemoryResource(ResourcePtr resource,
			const std::string &uri, const std::string &resource_name)
	:
	m_ResourceName(resource_name),
	m_MemoryInterface(mi::MemoryInterface::getInstance(uri)),
	m_Resource(resource)
	{
		init();
	}

	void XCFMemoryResource::init(){
		CBF_DEBUG("Subscription XPath: '" << memorySubscriptionXPath() << "'");
		mi::Condition condition(eventType, memorySubscriptionXPath());
		mi::TriggeredAction triggered_action(boost::bind(&XCFMemoryResource::triggered_action, this, _1));

		mi::Subscription subscription(condition, triggered_action);

		m_MemoryInterface -> subscribe(subscription);
	}

	void XCFMemoryResource::update(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ResourceMonitor);
		//First call update on the wrapped resource.
		m_Resource -> update();

		//Publish the new state of the resource.
		CBF_DEBUG("getting vector from the wraped resource");
		FloatVector stateVector = m_Resource -> get();

		CBF_DEBUG("writing vector into stream");
		std::stringstream stateVector_string;
		stateVector_string << stateVector;

		if(m_StateLocationPtr == NULL){
			CBF_DEBUG("creating vector doc");
			CBFSchema::BoostVector vectorDoc(stateVector_string.str());

			CBF_DEBUG("creating XCFMemoryResourceState doc");
			CBFSchema::XCFMemoryResourceState resourceState(m_ResourceName, vectorDoc);

			std::ostringstream s;
			CBFSchema::XCFMemoryResourceState_ (s, resourceState);

			CBF_DEBUG("document: " << s.str());

			//Insert the result-XML at the server.
			CBF_DEBUG("insert state at memory-server");
		
			std::string documentText = m_MemoryInterface -> insert(s.str());

			//Saving document for future state-updates.
			xmltio::XPath statePath(documentVectorLocationXPath());

			m_StateLocationPtr = 
			boost::shared_ptr<xmltio::Location>(new xmltio::Location(documentText,statePath));

		} else {
			//Edit the document and replace the one on the memory

			*m_StateLocationPtr = stateVector_string.str();

			CBF_DEBUG("Document text:" << m_StateLocationPtr -> getDocumentText());

			/*
			 * Because the xmltio::Location holds the whole document we can take the 
			 * document from the Location.
			*/
			m_MemoryInterface -> replace(m_StateLocationPtr -> getDocumentText());
		}
		
		
	}

	void XCFMemoryResource::triggered_action(const memory::interface::Event &event){
		CBF_DEBUG("in");
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ResourceMonitor);
		CBF_DEBUG("locked");

		CBF_DEBUG("doc: " << event.getDocument());

		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		std::istringstream s(documentText);
		std::auto_ptr<CBFSchema::XCFMemoryResourceAdd> resource = 
				CBFSchema::XCFMemoryResourceAdd_(s, xml_schema::flags::dont_validate);

		CBF_DEBUG("create vector");
		FloatVector resourceVector = create_vector(resource -> Vector());
		CBF_DEBUG("vector: " << resourceVector);

		CBF_DEBUG("vector created");
		if (resourceVector.size() != m_Resource -> dim()) {
			CBF_DEBUG("meeeh!!!");
			CBF_DEBUG("Dimensions of xml vector not matching the dimension of this resource. not adding");
		} else {
			CBF_DEBUG("adding vector to resource");
			m_Resource -> add(resourceVector);
		}
		CBF_DEBUG("out");
	}
} //namespace

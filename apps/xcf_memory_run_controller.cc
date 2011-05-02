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

#include <xcf_memory_run_controller.h>
#include <boost/bind.hpp>
#include <cbf/xsd_error_handler.h>
#include <cbf/xml_factory.h>

#include <cbf/namespace.h>

#include <memory>

namespace CBF {

namespace mi = memory::interface;

	XCFMemoryRunController::XCFMemoryRunController(
				std::string run_controller_name,
				std::string active_memory_name,
				NotificationLevel notification_level,
				unsigned int sleep_time,
				unsigned int steps,
				unsigned int verbosity_level
				#ifdef CBF_HAVE_QT
					,bool qt_support
				#endif
				)
		:
		m_RunControllerName(run_controller_name),
		m_MemoryInterface(mi::MemoryInterface::getInstance(active_memory_name)),
		m_NotificationLevel(notification_level),
		m_RunController(new CBFRunController(
			sleep_time,
			steps,
			verbosity_level
			#ifdef CBF_HAVE_QT
				,qt_support
			#endif
			)),
		m_DocumentMap()
	{
		// we only, listen to insert and replace events.
		unsigned int event = memory::interface::Event::INSERT | memory::interface::Event::REPLACE;

		CBF_DEBUG("Subscribing Add at active_memory");
		mi::Condition condition(event, addXPath());
		mi::TriggeredAction triggered_action(boost::bind(
				&XCFMemoryRunController::triggered_action_add, this, _1));
		mi::Subscription subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);

		CBF_DEBUG("Subscribing Options at active_memory");
		condition = mi::Condition(event, optionsXPath());
		triggered_action = mi::TriggeredAction(boost::bind(
				&XCFMemoryRunController::triggered_action_options, this, _1));
		subscription = mi::Subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);

		CBF_DEBUG("Subscribing Execute at active_memory");
		condition = mi::Condition(event, executeXPath());
		triggered_action = mi::TriggeredAction(boost::bind(
				&XCFMemoryRunController::triggered_action_execute, this, _1));
		subscription = mi::Subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);

		CBF_DEBUG("Subscribing Stop at active_memory");
		condition = mi::Condition(event, stopXPath());
		triggered_action = mi::TriggeredAction(boost::bind(
				&XCFMemoryRunController::triggered_action_stop, this, _1));
		subscription = mi::Subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);

		CBF_DEBUG("Subscribing LoadNamespace at active_memory");
		condition = mi::Condition(event, loadNamespaceXPath());
		triggered_action = mi::TriggeredAction(boost::bind(
				&XCFMemoryRunController::triggered_action_load_namespace, this, _1));
		subscription = mi::Subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);
	}

	void XCFMemoryRunController::triggered_action_add(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for attachments, adding");

		try {
			// a list of added documents
			std::vector<std::string> added_documents = std::vector<std::string>();
			// a list of pairs <ignored document name, reason>
			std::vector<std::string> ignored_documents = std::vector<std::string>();

			// parsing the XCFMemoryRunControllerAdd document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerAdd> controllerAdd = 
				CBFSchema::XCFMemoryRunControllerAdd_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("getting attachments and saving them");

			memory::interface::Attachments att;
			// TODO: event.hasAttachments() does not work.
			m_MemoryInterface -> getAttachments((event.getMemoryElement().asString()), att);
			for (mi::Attachments::const_iterator it = att.begin();
				it != att.end(); ++it){
				// getting data from attachment
				memory::interface::Buffer buff = it -> second;
				std::stringstream tmp;
				for (mi::Buffer::const_iterator it2 = buff.begin();
					it2 != buff.end(); ++it2) {
					tmp << *it2;
				}

				CBF_DEBUG("saving attachment[" << it -> first << "]");
				CBF_DEBUG("document: " << tmp.str());
				//test whether it is parsable.
				if(true){//test_initializable(tmp.str(), event.getID())){
					//document is parsable. adding
					m_DocumentMap[it -> first] = tmp.str();
					added_documents.push_back(it -> first);
				} else {
					//document not parsable -> ignorelist.
					ignored_documents.push_back(it -> first);
				}
			}

			CBF_DEBUG("notifying");
			notifyAdd(event.getID(), added_documents, ignored_documents);
			CBF_DEBUG("notifyed. returning.");
		} catch (const xml_schema::exception& e) {
			std::string note("An error occured during parsing: ");
			note.append(e.what());
			notifyError(note, event.getID());
		} catch (const std::exception& e) {
			std::string note("An unexpected exception occured: ");
			note.append(e.what());
			notifyError(note, event.getID());
		} catch (...) {
			notifyError("An unknown unexpected exception occured.", event.getID());
		}
	}

	void XCFMemoryRunController::triggered_action_options(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());

		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		bool time_set = false;
		bool steps_set = false;
		unsigned int time, steps;
		
		CBF_DEBUG("parsing XML for options");
		try {
			// parsing the XCFMemoryRunControllerOptions document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerOptions> controllerOpt = 
				CBFSchema::XCFMemoryRunControllerOptions_(s, xml_schema::flags::dont_validate);

			// if the SleepTime element exists set the sleep_time of the RunController
			if(controllerOpt -> SleepTime().present()){
				CBF_DEBUG("parsing XML for sleep time");
				time = controllerOpt -> SleepTime().get();
				CBF_DEBUG("changing sleep_time to: " << time);
				m_RunController -> setSleepTime(time);

				time_set = true;
			}

			// if the Steps element exists set the step_count of the RunController
			if(controllerOpt -> Steps().present()){
				CBF_DEBUG("parsing XML for steps");
				steps = controllerOpt -> Steps().get();
				CBF_DEBUG("changing step count to: " << steps);
				m_RunController -> setStepCount(steps);

				steps_set = true;
			}

		} catch (const xml_schema::exception& e) {
			std::string note("An error occured during parsing: ");
			note.append(e.what());
			notifyError(note, event.getID());
		} catch (const std::exception& e) {
			std::string note("An unexpected exception occured: ");
			note.append(e.what());
			notifyError(note, event.getID());
		} catch (...) {
			notifyError("An unknown unexpected exception occured.", event.getID());
		}
		
		notifyOptions(event.getID(), time, steps, time_set, steps_set);
		
	}

	void XCFMemoryRunController::triggered_action_execute(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();
		std::string controller_name;

		CBF_DEBUG("parsing XML for controller name");
		try {
			// parsing the XCFMemoryRunControllerExecute document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerExecute> controllerEx = 
				CBFSchema::XCFMemoryRunControllerExecute_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("parsing controller name");
			// get the name of the controller to run.
			controller_name = controllerEx -> ControllerName();

			CBF_DEBUG("starting controller called: " << controller_name);

			// run the specified controller.
			m_RunController -> start_controller(controller_name);
			notifyStop(event.getID(), controller_name);
			
		} catch (const ObjectNamespaceNotSetException& e){
			notifyError("Runtime Error: Cant execute controller when Namespace is not set." ,
				    event.getID());
		} catch (const ControllerNotFoundExcepption& e){
		  	std::ostringstream err;
			err << "Runtime Error: Cant find controller '";
			err << controller_name << "' in Namespace.";
			notifyError(err.str() , event.getID());
		} catch (const ControllerRunningException& e){
			notifyError("Runtime Error: Controller is already running." , event.getID());
		} catch (const xml_schema::exception& e) {
			notifyError(std::string("An error occured during parsing: ").append(e.what()), event.getID());
		} catch (const std::exception& e) {
			notifyError(std::string("An unexpected exception occured: ").append(e.what()), event.getID());
		} catch (...) {
			notifyError("An unknown unexpected exception occured.", event.getID());
		}
	}

	void XCFMemoryRunController::triggered_action_stop(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
		CBF_DEBUG("stopping controller");
		// just stop the controller
		m_RunController -> stop_controller();
	}

	void XCFMemoryRunController::triggered_action_load_namespace(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
		CBF::XSDErrorHandler err_handler;
		CBF::ObjectNamespacePtr object_namespace(new CBF::ObjectNamespace);
		int documents_added = 0;
		int documents_not_found = 0;
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for controller names");
		try {
			// parsing the XCFMemoryRunControllerLoadNamespace document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerLoadNamespace> loadNamespace =
				CBFSchema::XCFMemoryRunControllerLoadNamespace_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("parsing namespaces");
			// a set of documents that were not found.
			std::set<std::string> not_found_documents;
			// a set of documents that were loaded.
			std::set<std::string> loaded_documents;

			// for every attachment-name in this document
			CBF::ControlBasisPtr cb(new CBF::ControlBasis());
			CBFSchema::XCFMemoryRunControllerLoadNamespace::AttachmentName_sequence::const_iterator it;
			for (
				it = loadNamespace -> AttachmentName().begin();
				it != loadNamespace -> AttachmentName().end();
				++it
				)
			{
				// find the corresponding document in the map.
				// try to parse the document as an object and
				// initialize it in the object_namespace
				try {

					if(m_DocumentMap.find(*it) == m_DocumentMap.end()){
						documents_not_found++;
					} else{
						//getting the document that was mapped to the AttachmentName
						std::istringstream tmp(m_DocumentMap.find(*it) -> second);
						// Create a cbfschema::object
						std::auto_ptr<CBFSchema::Object> obj
							(CBFSchema::Object_
								(tmp, err_handler, xml_schema::flags::dont_validate));

						// Add objects to namespace
						CBF::ObjectPtr cb = CBF::XMLObjectFactory::instance()
											-> create<CBF::Object>(*obj, object_namespace);
						loaded_documents.insert(m_DocumentMap.find(*it) -> first);
						documents_added++;
					}
				} catch (const xml_schema::exception& e) {
					std::string note("Could not parse document as object: ");
					note.append(e.what());
					notifyError(note, event.getID());
				} catch (const std::exception& e) {
				  	std::string note("Could not parse document as object: ");
					note.append(e.what());
					notifyError(note, event.getID());
				} catch (...) {
					notifyError("An unknown unexpected exception occured while parsing "
						"Attachment as object.", event.getID());
				}
			}

			CBF_DEBUG("namespace created");

			if(documents_added > 0){
				CBF_DEBUG("setting the ObjectNamespace");
				// set the namespace
				m_RunController -> setObjectNamespace(object_namespace);
			}
			notifyLoad(event.getID(), loaded_documents, not_found_documents);
			
		} catch (const ControllerRunningException& e){
			notifyError("Error: Can not set ControlBasis while controller is running.", event.getID());
		} catch (const xml_schema::exception& e) {
			notifyError(std::string("An error occured during parsing: ").append(e.what()), event.getID());
		} catch (const std::exception& e) {
			notifyError(std::string("An unexpected exception occured: ").append(e.what()), event.getID());
		} catch (...) {
			notifyError("An unknown unexpected exception occured.", event.getID());
		}
	}
	
	void XCFMemoryRunController::notifyError(std::string note, int documentID){
		CBF_DEBUG(note);
		if (m_NotificationLevel & XCFMemoryRunController::ERROR){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			v.Note(note);

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}		 
	}
	
	void XCFMemoryRunController::notifyAdd(int documentID, std::vector<std::string> added_documents,
			std::vector<std::string> ignored_documents)
	{
		if (m_NotificationLevel & XCFMemoryRunController::INFO){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			//Adding information about added documents
			std::vector<std::string>::const_iterator it;
			for (it = added_documents.begin(); it != added_documents.end(); ++it)
			{
				 v.AddedDocumentName().push_back(*it);
			}
			//Adding information about ignored douments
			for (it = ignored_documents.begin(); it != ignored_documents.end(); ++it)
			{
				v.DocumentIgnoredName().push_back(*it);
			}

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}
		  
	}
	
	void XCFMemoryRunController::notifyLoad(int documentID, std::set<std::string> loaded_documents,
					std::set<std::string> not_found_documents)
	{
		if (m_NotificationLevel & XCFMemoryRunController::INFO){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			//Adding information about loaded documents
			for (std::set<std::string>::const_iterator it = loaded_documents.begin();
				it != loaded_documents.end(); ++it)
			{
				 v.LoadDocumentName().push_back(*it);
			}
			//Adding information about not found documents
			std::set<std::string>::const_iterator it;
			for (it = not_found_documents.begin(); it != not_found_documents.end(); ++it)
			{
				 v.NotFoundDocumentName().push_back(*it);
			}
			if(loaded_documents.size() == 0){
				v.Note("No namespace set because no document could be parsed.");
				CBF_DEBUG("No namespace set because no document could be parsed.");
			}

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}
	}

	void XCFMemoryRunController::notifyStop(int documentID, std::string controller_name){
		if (m_NotificationLevel & XCFMemoryRunController::INFO){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			v.StoppedControllerName(controller_name);
			v.StoppedControllerConverged(m_RunController -> checkConverged());

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}
	}
	
	void XCFMemoryRunController::notifyOptions(int documentID, unsigned int sleep_time,
						   unsigned int steps, bool time_set, bool steps_set)
	{
		if (m_NotificationLevel & XCFMemoryRunController::INFO){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			if(time_set){
				v.SleepTimeSetTo(sleep_time); 
			}
			if(steps_set){
				v.StepCountSetTo(steps); 
			}

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}
	}

	bool XCFMemoryRunController::test_initializable(std::string document, int eventID){
		// trying to parse the attachments as an object and
		// initialize them in the object_namespace
		CBF::ObjectNamespacePtr object_namespace(new CBF::ObjectNamespace);
		CBF::XSDErrorHandler err_handler;
		try {
			// Create a cbfschema::object
			std::stringstream doc(document);
			CBF_DEBUG("parse document for CBF::Object: " << document);
			std::auto_ptr<CBFSchema::Object> obj
				(CBFSchema::Object_
					(doc, err_handler, xml_schema::flags::dont_validate));


			// Add objects to namespace
			CBF_DEBUG("add object to namespace");
			CBF::ObjectPtr cb = CBF::XMLObjectFactory::instance()
									-> create<CBF::Object>(*obj, object_namespace);

			return true; //everything okay
		} catch (const xml_schema::exception& e) {
			std::string note("Could not parse Buffer as CBF::Object ");
			note.append(e.what());
			notifyError(note, eventID);
		} catch (const std::exception& e) {
			std::string note("Could not parse Buffer as CBF::Object: ");
			note.append(e.what());
			notifyError(note, eventID);
		} catch (...) {
			notifyError("An unknown unexpected exception occurred while parsing "
					"Attachment as CBF::Object.", eventID);
		}
		return false;
	}

} //namespace

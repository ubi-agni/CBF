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
		m_ControllerMap()
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

		CBF_DEBUG("Subscribing LoadControllers at active_memory");
		condition = mi::Condition(event, loadControllersXPath());
		triggered_action = mi::TriggeredAction(boost::bind(
				&XCFMemoryRunController::triggered_action_load_controllers, this, _1));
		subscription = mi::Subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);
	}

	void XCFMemoryRunController::triggered_action_add(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for controllers, adding");

		try {
			//we will count the controllers and save theyr names, so we can notify them.
			int ignored_controllers = 0;
			std::set<std::string> added_controller_names;
			std::map<std::string, int> overwritten_controller_names;

			// parsing the XCFMemoryRunControllerAdd document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerAdd> controllerAdd = 
				CBFSchema::XCFMemoryRunControllerAdd_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("parsing XML for optional ControlBasis");
			if(controllerAdd -> ControlBasis().present()){
			// if a ControlBasis exists.
				CBFSchema::ControlBasis cb = controllerAdd -> ControlBasis().get();
				std::string name;
				// adding every controller from the control_basis.
				add_controllers_to_map(&(controllerAdd -> ControlBasis().get().Controller()), 
				&added_controller_names, &overwritten_controller_names, &ignored_controllers);
			}

			CBF::XSDErrorHandler err_handler;
			CBF_DEBUG("getting attachments and parsing them");
			if(controllerAdd -> Attachments().present() && controllerAdd -> Attachments().get()) {
				memory::interface::Attachments att;
				// TODO: event.hasAttachments() always returns 0. This way works with UUIS's
				// on the publishers side. Somehow redundant namings return the attachments
				// of old documents.
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

					CBF_DEBUG("creating a control_basis from attachment[" << it -> first << "]");
					CBF_DEBUG("control_basis: " << tmp.str());

					// trying to parse the attachments as a control_basis
					try {
						std::auto_ptr<CBFSchema::ControlBasis> cbt
							(CBFSchema::ControlBasis_
								(tmp, err_handler, xml_schema::flags::dont_validate));

						// adding every controller from the control_basis.
						add_controllers_to_map(&(cbt -> Controller()), &added_controller_names,
									&overwritten_controller_names, 
									&ignored_controllers);

					} catch (const xml_schema::exception& e) {
						std::string note("Could not parse Buffer as control_basis: ");
						note.append(e.what());
						notifyError(note, event.getID());
					} catch (const std::exception& e) {
					  	std::string note("Could not parse Buffer as control_basis: ");
						note.append(e.what());
						notifyError(note, event.getID());
					} catch (...) {
						notifyError("An unknown unexpected exception occured while parsing "
							"Attachment as control_basis.", event.getID());
					}
				}
			}

			notifyAdd(event.getID(), added_controller_names, 
					overwritten_controller_names, ignored_controllers);
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

	void XCFMemoryRunController::add_controllers_to_map(
			CBFSchema::ControlBasis::Controller_sequence* controllers,
			std::set<std::string>* added_controllers, 
			std::map<std::string, int>* overwritten_controllers,
			int* ignored_controllers_count
			)
	{
		for (CBFSchema::ControlBasis::Controller_sequence::const_iterator it = controllers -> begin();
			it != controllers -> end();
			++it)
		{
			// ignoring controllers without a name.
			if(it -> Name().present()){
				std::string name = it -> Name().get();
				//does a controller with that name already exist?
				// a second controller with the same name will overwrite the first.
				if (m_ControllerMap.find(name) != m_ControllerMap.end()){
					if(overwritten_controllers -> find(name) != overwritten_controllers -> end()) {
					//controller has been overwritten multiple times
						(*overwritten_controllers)[name]++;
					} else {
					//controller is overwritten first time
						(*overwritten_controllers)[name] = 1;
					}
				}
				
				// saving a copy of the controller in a map.
				// the copy must be polymorphic.
				// the controller is mapped to its name.
				m_ControllerMap[name] = 
					boost::shared_ptr<CBFSchema::Controller>(it -> _clone());
				CBF_DEBUG("added controller named: " << name);
			
				added_controllers -> insert(name);
				
			} else {
				CBF_DEBUG("controller has no name. ignoring");
				(*ignored_controllers_count)++;
			}
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
			
		} catch (const ControlBasisNotSetException& e){
			notifyError("Runtime Error: Cant execute controller when ControlBasis is not set." ,
				    event.getID());
		} catch (const ControllerNotFoundExcepption& e){
		  	std::ostringstream err;
			err << "Runtime Error: Cant find controller '";
			err << controller_name << "' in ControlBasis.";
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

	void XCFMemoryRunController::triggered_action_load_controllers(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for controller names");
		try {
			// parsing the XCFMemoryRunControllerLoadControllers document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerLoadControllers> controllerLC = 
				CBFSchema::XCFMemoryRunControllerLoadControllers_(s, xml_schema::flags::dont_validate);

			// we fill this controll_basis with controllers and then we will 
			// create a CBFControlBasis from it.
			CBFSchema::ControlBasis controlBasis;

			CBF_DEBUG("parsing controller names");
			// a set of controller_names that were not found.
			std::set<std::string> not_found_controllers;
			// a set of controller_names that were loaded.
			std::set<std::string> loaded_controllers;
			// for every controller_name in the document
			for (CBFSchema::XCFMemoryRunControllerLoadControllers::
					ControllerName_sequence::const_iterator it 
					= controllerLC -> ControllerName().begin();
				it != controllerLC -> ControllerName().end();
				++it)
			{
				// find the corrresponding controller in our map.
				if (m_ControllerMap.find(*it) != m_ControllerMap.end()) {
					// add controller to control_basis
					controlBasis.Controller().push_back(*(m_ControllerMap[*it]));
					loaded_controllers.insert(*it);
				} else {
					CBF_DEBUG("Could not find controller named: " << *it << ". Ignoring.");
					not_found_controllers.insert(*it);
				}
			}

			std::ostringstream t;
			CBFSchema::ControlBasis_ (t, controlBasis);
			
			CBF_DEBUG("creating control_basis" << t.str());
			//init the CBF::ControlBasis
			ObjectNamespacePtr object_namespace(new ObjectNamespace);
			CBF::ControlBasisPtr control_basis(new CBF::ControlBasis(controlBasis, object_namespace));
			CBF_DEBUG("control_basis created");

			if(control_basis -> controllers().size() > 0){
				CBF_DEBUG("setting the control_basis");
				// set the control_basis
				m_RunController -> setControlBasis(control_basis);
			}
			notifyLoad(event.getID(), loaded_controllers, not_found_controllers);
			
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
	
	void XCFMemoryRunController::notifyAdd(
					int documentID, std::set<std::string> added_controller_names,
					std::map<std::string, int> overwritten_controllers,
					int ignored_controllers_count)
	{
		if (m_NotificationLevel & XCFMemoryRunController::INFO){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			//Adding information about added controllers
			for (std::set<std::string>::const_iterator it = added_controller_names.begin();
				it != added_controller_names.end(); ++it)
			{
				 v.AddedControllerName().push_back(*it);
			}
			//Adding information about overwritten controllers
			for (std::map<std::string, int>::const_iterator it = overwritten_controllers.begin();
				it != overwritten_controllers.end(); ++it)
			{
				std::ostringstream tmp;
				if (it -> second > 1) { tmp << it -> second << "*";}
				tmp << it -> first;
				v.OverwrittenControllerName().push_back(tmp.str());
			}
			//Adding information about iignored controllers
			v.ControllersIgnored(ignored_controllers_count);

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}
		  
	}
	
	void XCFMemoryRunController::notifyLoad(int documentID, std::set<std::string> loaded_controllers,
					std::set<std::string> not_found_controllers)
	{
		if (m_NotificationLevel & XCFMemoryRunController::INFO){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, documentID);

			//Adding information about loaded controllers
			for (std::set<std::string>::const_iterator it = loaded_controllers.begin();
				it != loaded_controllers.end(); ++it)
			{
				 v.LoadControllerName().push_back(*it);
			}
			//Adding information about not found controllers
			for (std::set<std::string>::const_iterator it = not_found_controllers.begin();
				it != not_found_controllers.end(); ++it)
			{
				 v.NotFoundControllerName().push_back(*it);
			}
			if(loaded_controllers.size() == 0){
				v.Note("No control_basis set because the resulting control_basis was empty.");
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

} //namespace
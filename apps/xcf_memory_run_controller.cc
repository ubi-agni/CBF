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
			m_Added = 0;
			m_Ignored = 0;
			std::ostringstream names;
			std::ostringstream overwritten;
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
				&names, &overwritten);
			}

			CBF::XSDErrorHandler err_handler;
			CBF_DEBUG("getting attachments and parsing them");
			if(controllerAdd -> Attachments().present() && controllerAdd -> Attachments().get()) {
				memory::interface::Attachments att;
				m_MemoryInterface -> getAttachments((event.getMemoryElement().asString()), att);
				for (mi::Attachments::const_iterator it = att.begin();
					it != att.end(); ++it){
					// getting data from attachment
					memory::interface::Buffer buff = it -> second;
					std::stringstream tmp;
					for (mi::Buffer::const_iterator it = buff.begin();
						it != buff.end(); ++it) {
						tmp << *it;
					}
					std::cout << tmp.str() << std::endl;

					CBF_DEBUG("creating a control_basis");
					// trying to parse the attachments as a control_basis
					try {
						std::auto_ptr<CBFSchema::ControlBasis> cbt
							(CBFSchema::ControlBasis_
								(tmp, err_handler, xml_schema::flags::dont_validate));
						// adding every controller from the control_basis.
						add_controllers_to_map(&(cbt -> Controller()), &names, &overwritten);
					} catch(...) {
					 	CBF_DEBUG("could not create control_basis");;
					}
				}
			}

			std::string nameStr = names.str();
			std::string overwrittenStr = overwritten.str();
			// removing the last space character from names-strings
			if(nameStr.size() != 0){
				nameStr.resize(nameStr.size() - 1);
			}
			if(overwrittenStr.size() != 0){
				overwrittenStr.resize(overwrittenStr.size() - 1);
			} else {
				overwrittenStr = "0";
			}


			std::ostringstream note;
			note << "Controllers Added(" << m_Added << ":"
				<< nameStr << "), Controllers Overwritten(" 
				<< overwrittenStr << "), Controllers Ignored(" 
				<< m_Ignored << ")";
			notify(note.str() , event.getID(), XCFMemoryRunController::INFO);
		} catch (const xml_schema::exception& e) {
			CBF_DEBUG("An error occured during parsing: " << e.what());
			notify(std::string("An error occured during parsing: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const std::exception& e) {
			CBF_DEBUG("An unexpected exception occured: " << e.what());
			notify(std::string("An unexpected exception occured: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (...) {
			CBF_DEBUG("An unknown unexpected exception occured.");
			notify("An unknown unexpected exception occured.", 
				event.getID(), XCFMemoryRunController::ERROR);
		}
	}

	void XCFMemoryRunController::add_controllers_to_map(
		CBFSchema::ControlBasis::Controller_sequence* controllers,
		std::ostringstream* names, std::ostringstream* overwritten)
	{
		for (CBFSchema::ControlBasis::Controller_sequence::const_iterator it = controllers -> begin();
			it != controllers -> end();
			++it)
		{
			// ignoring controllers without a name.
			if(it -> Name().present()){
				std::string name = it -> Name().get();
				//notifying if a controller with name already exists.
				if (m_ControllerMap.find(name) != m_ControllerMap.end()){
					*overwritten << name << ",";
				}
	
				// saving a copy of the controller in a map.
				// the copy must be polymorphic.
				// the controller is mapped to its name.
				// a second controller with the same name will overwrite the first.
				m_ControllerMap[name] = 
					boost::shared_ptr<CBFSchema::Controller>(it -> _clone());
				CBF_DEBUG("added controller named: " << name);
			
				*names << name << ",";
				m_Added++;
			} else {
				CBF_DEBUG("controller has no name. ignoring");
				m_Ignored++;
			}
		}
	}

	void XCFMemoryRunController::triggered_action_options(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());

		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for options");
		try {
			// parsing the XCFMemoryRunControllerOptions document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerOptions> controllerOpt = 
				CBFSchema::XCFMemoryRunControllerOptions_(s, xml_schema::flags::dont_validate);

			// if the SleepTime element exists set the sleep_time of the RunController
			if(controllerOpt -> SleepTime().present()){
				CBF_DEBUG("parsing XML for sleep time");
				unsigned int time = controllerOpt -> SleepTime().get();
				CBF_DEBUG("changing sleep_time to: " << time);
				m_RunController -> setSleepTime(time);
				
				std::ostringstream note;
				note << "Sleep time changed to: " << time << "ms.";

				notify(note.str() , event.getID(), XCFMemoryRunController::INFO);
			}

			// if the Steps element exists set the step_count of the RunController
			if(controllerOpt -> Steps().present()){
				CBF_DEBUG("parsing XML for steps");
				unsigned int steps = controllerOpt -> Steps().get();
				CBF_DEBUG("changing step count to: " << steps);
				m_RunController -> setStepCount(steps);

				std::ostringstream note;
				note << "Step count changed to: " << steps << ".";
				notify(note.str() , event.getID(), XCFMemoryRunController::INFO);
			}

		} catch (const xml_schema::exception& e) {
			CBF_DEBUG("An error occured during parsing: " << e.what());
			notify(std::string("An error occured during parsing: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const std::exception& e) {
			CBF_DEBUG("An unexpected exception occured: " << e.what());
			notify(std::string("An unexpected exception occured: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (...) {
			CBF_DEBUG("An unknown unexpected exception occured.");
			notify("An unknown unexpected exception occured.", 
				event.getID(), XCFMemoryRunController::ERROR);
		}
	}

	void XCFMemoryRunController::triggered_action_execute(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for controller name");
		try {
			// parsing the XCFMemoryRunControllerExecute document.
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerExecute> controllerEx = 
				CBFSchema::XCFMemoryRunControllerExecute_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("parsing controller name");
			// get the name of the controller to run.
			std::string controller_name = controllerEx -> ControllerName();

			CBF_DEBUG("starting controller called: " << controller_name);

			// run the specified controller.
			m_RunController -> start_controller(controller_name);

			std::ostringstream note;
			note << "Trying to start controller: " << controller_name << " .";
			notify(note.str() , event.getID(), XCFMemoryRunController::INFO);
		} catch (const ControlBasisNotSetException& e){
			CBF_DEBUG("Runtime Error: Cant execute controller when ControlBasis is not set.");
			notify("Runtime Error: Cant execute controller when ControlBasis is not set." ,
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const ControllerNotFoundExcepption& e){
			CBF_DEBUG("Runtime Error: Cant find controller in ControlBasis.");
			notify("Runtime Error: Cant find controller in ControlBasis." ,
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const ControllerRunningException& e){
			CBF_DEBUG("Runtime Error: Controller is already running.");
			notify("Runtime Error: Controller is already running." ,
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const xml_schema::exception& e) {
			CBF_DEBUG("An error occured during parsing: " << e.what());
			notify(std::string("An error occured during parsing: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const std::exception& e) {
			CBF_DEBUG("An unexpected exception occured: " << e.what());
			notify(std::string("An unexpected exception occured: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (...) {
			CBF_DEBUG("An unknown unexpected exception occured.");
			notify("An unknown unexpected exception occured.", 
				event.getID(), XCFMemoryRunController::ERROR);
		}
	}

	void XCFMemoryRunController::triggered_action_stop(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
		CBF_DEBUG("stopping controller");
		// just stop the controller
		m_RunController -> stop_controller();
		notify("Controller stopped." , event.getID()
			, XCFMemoryRunController::INFO);
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
			// for every controller_name in the document
			std::ostringstream note;
			note << "Could not find controllers named: ";
			unsigned int not_found_count = 0;
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
				} else {
					CBF_DEBUG("Could not find controller named: " << *it << ". Ignoring.");
					note << *it << " ";
					not_found_count++;
				}
			}
			note << ".";
			if(not_found_count){
				notify(note.str() , event.getID(), XCFMemoryRunController::INFO);
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
			} else {
				CBF_DEBUG("not setting control_basis because basis is empty");

				notify("Created ControlBasis is empty. Not setting/replacing ControlBasis." ,
					event.getID(), XCFMemoryRunController::ERROR);
			}
		} catch (const ControllerRunningException& e){
			CBF_DEBUG(e.what());
			notify("Error: Can not set ControlBasis while controller is running.",
				 event.getID(), XCFMemoryRunController::ERROR);
		} catch (const xml_schema::exception& e) {
			CBF_DEBUG("An error occured during parsing: " << e.what());
			notify(std::string("An error occured during parsing: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (const std::exception& e) {
			CBF_DEBUG("An unexpected exception occured: " << e.what());
			notify(std::string("An unexpected exception occured: ").append(e.what()), 
				event.getID(), XCFMemoryRunController::ERROR);
		} catch (...) {
			CBF_DEBUG("An unknown unexpected exception occured.");
			notify("An unknown unexpected exception occured.", 
				event.getID(), XCFMemoryRunController::ERROR);
		}
	}

	void XCFMemoryRunController::notify(std::string note, int documentID, 
			XCFMemoryRunController::NotificationLevel notification_level) {
		if (m_NotificationLevel & notification_level){
			// creating the XCFMemoryRunControllerNotification document.
			CBFSchema::XCFMemoryRunControllerNotification v(m_RunControllerName, note, documentID);

			std::ostringstream s;
			CBFSchema::XCFMemoryRunControllerNotification_ (s, v);
			// sending the document to the active_memory
			m_MemoryInterface -> send(s.str());
		}
	}
} //namespace
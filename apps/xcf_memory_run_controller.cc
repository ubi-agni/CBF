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
#include <cbf/schemas.hxx>

#include <memory>

namespace CBF {

namespace mi = memory::interface;

	XCFMemoryRunController::XCFMemoryRunController(
				std::string run_controller_name,
				std::string active_memory_name,
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
		unsigned int event = memory::interface::Event::INSERT | memory::interface::Event::REPLACE;

		CBF_DEBUG("Subscribing Add at active_memory");
		mi::Condition condition(event, addXPath());
		mi::TriggeredAction triggered_action(boost::bind(
				&XCFMemoryRunController::triggered_action_add, this, _1));
		mi::Subscription subscription(condition, triggered_action);
		m_MemoryInterface -> subscribe(subscription);

		CBF_DEBUG("Subscribing Options at active_memory");
		CBF_DEBUG("Options XPath: " << optionsXPath());
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
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerAdd> controllerAdd = 
				CBFSchema::XCFMemoryRunControllerAdd_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("parsing XML for optional ControlBasis");
			if(controllerAdd -> ControlBasis().present()){
				CBFSchema::ControlBasis cb = controllerAdd -> ControlBasis().get();
				std::string name;

				for (CBFSchema::ControlBasis::Controller_const_iterator it =
					cb.Controller().begin();
					it != cb.Controller().end();
					++it)
				{
					if(it -> Name().present()){
						std::string name = it -> Name().get();					

						m_ControllerMap[name] = 
							boost::shared_ptr<CBFSchema::Controller>(it -> _clone());

						CBF_DEBUG("added controller named: " << name);
					} else {
						CBF_DEBUG("controller has no name. ignoring");
					}
				}
			}

			CBF_DEBUG("parsing XML for list of controllers");
			for (CBFSchema::XCFMemoryRunControllerAdd::Controller_sequence::const_iterator it 
					= controllerAdd -> Controller().begin();
				it != controllerAdd -> Controller().end();
				++it)
			{
				if(it -> Name().present()){
					std::string name = it -> Name().get();					

					m_ControllerMap[name] = 
						boost::shared_ptr<CBFSchema::Controller>(it -> _clone());

					CBF_DEBUG("added controller named: " << name);
				} else {
					CBF_DEBUG("controller has no name. ignoring");
				}
			}
		} catch (...){
			CBF_DEBUG("Could not parse document as XCFMemoryRunControllerAdd");
		}
	}

	void XCFMemoryRunController::triggered_action_options(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for options");
		try {
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerOptions> controllerOpt = 
				CBFSchema::XCFMemoryRunControllerOptions_(s, xml_schema::flags::dont_validate);

			if(controllerOpt -> SleepTime().present()){
				CBF_DEBUG("parsing XML for sleep time");
				unsigned int time = controllerOpt -> SleepTime().get();
				CBF_DEBUG("changing sleep_time to: " << time);
				m_RunController -> setSleepTime(time);
			}

			if(controllerOpt -> Steps().present()){
				CBF_DEBUG("parsing XML for steps");
				unsigned int steps = controllerOpt -> Steps().get();
				CBF_DEBUG("changing step count to: " << steps);
				m_RunController -> setStepCount(steps);
			}

		} catch (...){
			CBF_DEBUG("Could not parse document as XCFMemoryRunControllerOptions");
		}
	}

	void XCFMemoryRunController::triggered_action_execute(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for controller name");
		try {
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerExecute> controllerEx = 
				CBFSchema::XCFMemoryRunControllerExecute_(s, xml_schema::flags::dont_validate);

			CBF_DEBUG("parsing controller name");
			std::string controller_name = controllerEx -> ControllerName();

			CBF_DEBUG("starting controller called: " << controller_name);

			try {
				m_RunController -> start_controller(controller_name);
			} catch (...) {
				CBF_DEBUG("Controller: " << controller_name << " not in control_basis");
			}
		} catch (...){
			CBF_DEBUG("Could not parse document as XCFMemoryRunControllerExecute");
		}
	}

	void XCFMemoryRunController::triggered_action_stop(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
		CBF_DEBUG("stopping controller");
		m_RunController -> stop_controller();
	}

	void XCFMemoryRunController::triggered_action_load_controllers(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		CBF_DEBUG("parsing XML for controller names");
		try {
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunControllerLoadControllers> controllerLC = 
				CBFSchema::XCFMemoryRunControllerLoadControllers_(s, xml_schema::flags::dont_validate);

			CBFSchema::ControlBasis controlBasis;

			CBF_DEBUG("parsing controller names");
			for (CBFSchema::XCFMemoryRunControllerLoadControllers::
					ControllerName_sequence::const_iterator it 
					= controllerLC -> ControllerName().begin();
				it != controllerLC -> ControllerName().end();
				++it)
			{
				if (m_ControllerMap.find(*it) != m_ControllerMap.end()) {

					controlBasis.Controller().push_back(*(m_ControllerMap[*it]));	
				} else {
					CBF_DEBUG("Could not find controller named: " << *it << ". Ignoring.");
				}
			}

			std::ostringstream t;
			CBFSchema::ControlBasis_ (t, controlBasis);
			
			CBF_DEBUG("creating control_basis" << t.str());
			CBF::ControlBasisPtr control_basis(new CBF::ControlBasis(controlBasis));
			CBF_DEBUG("control_basis created");

			if(control_basis -> controllers().size() > 0){
				CBF_DEBUG("setting the control_basis");
				m_RunController -> setControlBasis(control_basis);
			} else {
				CBF_DEBUG("not setting control_basis because basis is empty");
			}
		} catch (const xml_schema::exception& e){
			CBF_DEBUG(e);
		}
	}


} //namespace

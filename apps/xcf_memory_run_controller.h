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

#ifndef XCF_MEMORY_RUN_CONTROLLER_HH
#define XCF_MEMORY_RUN_CONTROLLER_HH

#include <cbf/config.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/debug_macros.h>

#include <cbf_run_controller.h>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

#ifdef CBF_HAVE_QT
	#include <QtGui/QApplication>
#endif

#include <Memory/Condition.hpp>
#include <Memory/Subscription.hpp>
#include <Memory/Interface.hpp>
#include <Memory/Event.hpp>

#include <memory>
#include <map>

namespace CBF {

/**
	@brief A struct that runs a controller from an XCFMemoryRunController 
	document provided by an active_memory.
*/
struct XCFMemoryRunController {

	public:

	/**
		@brief This struct connects to an active_memory and subscribes for
		XCFMemoryRunController-documents with the specified RunController-name.
		When a document ist inserted, the XCFMemoryRunController performs the
		corresponding actions.

		@param controller_name The name of the RunController to listen for.
		@param active_memory_name The name of the active_memory to connect to.
		@param run_controller The CBFRunController on which the execution is performed.
				
	*/
	XCFMemoryRunController(std::string run_controller_name, std::string active_memory_name,
				unsigned int sleep_time = 0,
				unsigned int steps = 0,
				unsigned int verbosity_level = 0
				#ifdef CBF_HAVE_QT
					,bool qt_support = false
				#endif
				);

	/**
		A virtual destructor, so polymorphic cleanup 
		works nicely
	*/
	virtual ~XCFMemoryRunController() {}

	private:

	/**
		@brief Holds the name of the XCFMemoryRunController to listen to.
	*/
	std::string m_RunControllerName;

	/**
		@brief Holds the pointer to the MemoryInterface.
	*/
	memory::interface::MemoryInterface::pointer m_MemoryInterface;

	/**
		@brief Holds the CBFRunController which is used for the execution..
	*/
	CBFRunControllerPtr m_RunController;

	/**
		@brief A map that holds controllers identified by their names.
	*/
	std::map<std::string, CBFSchema::Controller> m_ControllerMap;

	/**
		@brief Returns the string that identifies the XML-document that
		adds controllers.
	*/
	const char* addXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerAdd[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str().c_str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		sets the Controllers sleep-time and steps.
	*/
	const char* optionsXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerOptions[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str().c_str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		starts the execution of a controller.
	*/
	const char* executeXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerExecute[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str().c_str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		stops execution of a controller.
	*/
	const char* stopXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerStop[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str().c_str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		loads controllers as a control_basis.
	*/
	const char* loadControllersXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerLoadControllers[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str().c_str();
	}

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerAdd document is available. It Adds
		controllers to the m_ControllerMap.
	*/
	void triggered_action_add(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerAdd document is available. It Adds
		controllers to the m_ControllerMap.
	*/
	void triggered_action_options(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerAdd document is available. It Adds
		controllers to the m_ControllerMap.
	*/
	void triggered_action_execute(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerAdd document is available. It Adds
		controllers to the m_ControllerMap.
	*/
	void triggered_action_stop(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerAdd document is available. It Adds
		controllers to the m_ControllerMap.
	*/
	void triggered_action_load_controllers(const memory::interface::Event &event);

};

/**
	@brief A shared pointer to a CBFRunController.
*/
typedef boost::shared_ptr<XCFMemoryRunController> XCFMemoryRunControllerPtr;


} //namespace
#endif

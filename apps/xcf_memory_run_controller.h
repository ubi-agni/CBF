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

#include <cbf/schemas.hxx>

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
	It reacts on the following xml-documents:
		XCFMemoryRunController[Add | Options | Execute | Stop | LoadControllers]
*/
struct XCFMemoryRunController {

	public:

	enum NotificationLevel { NOTHING = 0x0 , ERROR = 0x1, INFO = 0x2, ALL = 0x3};

	/**
		@brief This struct connects to an active_memory and subscribes for
		XCFMemoryRunController-documents with the specified RunController-name.
		When a document ist inserted, the XCFMemoryRunController performs the
		corresponding actions.

		@param run_controller_name The name of the RunController to listen for.
		@param active_memory_name The name of the active_memory to connect to.
		@param sleep-time (CBFRunController) The sleep time after each step of execution in milliseconds.
		@param steps (CBFRunController) The count of steps to execute. Setting steps to 0 lets the controller run
		until convergence.
		@param (CBFRunController) verbosity_level Sets the verbosity level.
		@param qt_support (CBFRunController) Switches the qt-support. Only available when Qt was found.
		A QApplication needs to be created by the initializing program.
				
	*/
	XCFMemoryRunController(std::string run_controller_name, std::string active_memory_name,
				NotificationLevel notification_level = NOTHING,
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
		@brief Tells which type of notifications will be send.
	*/
	NotificationLevel m_NotificationLevel;

	/**
		@brief Holds the CBFRunController which is used for the execution..
	*/
	CBFRunControllerPtr m_RunController;

	/**
		@brief A map that holds controllers identified by their names.
	*/
	std::map<std::string, boost::shared_ptr<CBFSchema::Controller> > m_ControllerMap;

	/**
		@brief A counter that is used for Notifications.
	*/
	unsigned int m_Added;

	/**
		@brief A counter that is used for Notifications.
	*/
	unsigned int m_Ignored;

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
		xPathStream << "/p1:XCFMemoryRunControllerOptions [RunControllerName='";
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
		@brief A helper-function for triggered_action_add().
	*/
	void add_controllers_to_map(
		CBFSchema::ControlBasis::Controller_sequence* controllers,
		std::ostringstream* names, std::ostringstream* overwritten);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerOptions document is available. It sets
		the sleep time and steps.
	*/
	void triggered_action_options(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerExecute document is available. It starts
		the execution of a controller.
	*/
	void triggered_action_execute(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerStop document is available. It stops
		the execution of a controller.
	*/
	void triggered_action_stop(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerLoadControllers document is available. It 
		creates a control_basis and sets it in m_RunController.
	*/
	void triggered_action_load_controllers(const memory::interface::Event &event);

	/**
		@brief Sends (not inserts) an XCFMemoryRunControllerNotification document.
		Whether a notification will be send defines the m_NotificationLevel.

		@param note Will be filled into the 'Note' element.
		@param documentD The dbxml:id of the xml document that triggered this note.
		@param notification_level The NL of this notification
	*/
	void notify(std::string note, int documentID, NotificationLevel notification_level);

};

/**
	@brief A shared pointer to a XCFMemoryRunController.
*/
typedef boost::shared_ptr<XCFMemoryRunController> XCFMemoryRunControllerPtr;


} //namespace
#endif

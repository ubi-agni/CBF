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
		@brief Returns the string that identifies the XML-document that
		adds controllers.
	*/
	const std::string addXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerAdd[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		sets the Controllers sleep-time and steps.
	*/
	const std::string optionsXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerOptions [RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		starts the execution of a controller.
	*/
	const std::string executeXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerExecute[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		stops execution of a controller.
	*/
	const std::string stopXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerStop[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str();
	}
	/**
		@brief Returns the string that identifies the XML-document that
		loads controllers as a control_basis.
	*/
	const std::string loadControllersXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerLoadControllers[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str();
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
			std::set<std::string>* added_controllers, 
			std::map<std::string, int>* overwritten_controllers,
			int* ignored_controllers_count);

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
		@brief Sends (not inserts) an error XCFMemoryRunControllerNotification document.
		Whether a notification will be send defines the m_NotificationLevel.

		@param note Will be filled into the 'Note' element.
		@param documentD The dbxml:id of the xml document that triggered this note.
	*/
	void notifyError(std::string note, int documentID);
	
	/**
		@brief A notify function for the add-action.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentD The dbxml:id of the xml document that triggered this note.
		@param added_controler_names The set of controller_names that were added.
		@param overwritten_controllers The map of overwritten controllers and with the count of
		overwritings.
		@param ignored_controllers_count The count of controllers that were ignored.
	*/
	void notifyAdd(int documentID, std::set<std::string> added_controller_names,
					std::map<std::string, int> overwritten_controllers,
					int ignored_controllers_count);

	/**
		@brief A notify function for the load-action.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentD The dbxml:id of the xml document that triggered this note.
		@param Ok Has a control_basis been set?
		@param not_found_controllers A set of controller_names that were not found.
	*/
	void notifyLoad(int documentID, std::set<std::string> loaded_controllers,
			std::set<std::string> not_found_controllers);

	/**
		@brief A notify function for the stop-action.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentD The dbxml:id of the xml document that triggered this note.
		@param controller_name The name of the controller that was stopped.
	*/
	void notifyStop(int documentID, std::string controller_name);
	
	/**
		@brief A notify function for the sleep-time/steps-change.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentD The dbxml:id of the xml document that triggered this note.
		@param sleep_time The new sleep-time.
		@param steps The new amount of steps.
		@param time_set Whether the time was changed.
		@param steps_set Whether the steps were changed.
	*/
	void notifyOptions(int documentID, unsigned int sleep_time, unsigned int steps, bool time_set, bool steps_set);
	
};

/**
	@brief A shared pointer to a XCFMemoryRunController.
*/
typedef boost::shared_ptr<XCFMemoryRunController> XCFMemoryRunControllerPtr;


} //namespace
#endif

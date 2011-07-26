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

#ifdef CBF_HAVE_QT
	#include <QtGui/QApplication>
	#include <QWaitCondition>
	#include <QMutexLocker>
	#include <QMutex>
#endif

#include <Memory/Condition.hpp>
#include <Memory/Subscription.hpp>
#include <Memory/Interface.hpp>
#include <Memory/Event.hpp>

#include <memory>
#include <map>
#include <queue>

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

	#ifdef CBF_HAVE_QT
		enum Function { ADD = 0x4 , LOAD = 0x5, EXECUTE = 0x6};
	#endif

	/**
		@brief This struct connects to an active_memory and subscribes for
		XCFMemoryRunController-documents with the specified RunController-name.
		When a document is inserted, the XCFMemoryRunController performs the
		corresponding actions.

		@param run_controller_name The name of the RunController to listen for.
		@param active_memory_name The name of the active_memory to connect to.
		@param notification_level The notification level defines which messages are inserted into memory.
		@param sleep_time (CBFRunController) The sleep time after each step of execution in milliseconds.
		@param steps (CBFRunController) The count of steps to execute. Setting steps to 0 lets the controller run
		until convergence.
		@param verbosity_level (CBFRunController) verbosity_level Sets the verbosity level.
	*/
	XCFMemoryRunController(std::string run_controller_name, std::string active_memory_name,
				NotificationLevel notification_level = NOTHING,
				unsigned int sleep_time = 0,
				unsigned int steps = 0,
				unsigned int verbosity_level = 0
				#ifdef CBF_HAVE_QT
					,bool qt_support = false
					,QWaitCondition* wait_condition = 0
				#endif
				);

	/**
		A virtual destructor, so polymorphic cleanup 
		works nicely
	*/
	virtual ~XCFMemoryRunController() {}

	#ifdef CBF_HAVE_QT
		/**
			@brief This function is called by the main thread and handles the events
			from the event-queue.
		 */
		void handle_events();
	#endif

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
		@brief A map that holds received CBFObject-documents as std::string
		identified by their attachment-id.
	*/
	std::map<std::string, std::string> m_DocumentMap;

	#ifdef CBF_HAVE_QT
		/**
			@brief This is used for thread synchronisation when Qt is available.
		 */
		QWaitCondition* m_WaitCondition;

		/**
			@brief This queue holds the events which need to be handled by the
			applications main thread.
		*/
		std::queue<std::pair<Function, memory::interface::Event> > m_EventQueue;

		/**
			@brief This mutex-lock is used to synchronise the m_EventQueue
		*/
		QMutex m_EventQueueLock;

	#endif

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
	const std::string loadNamespaceXPath(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunControllerLoadNamespace[RunControllerName='";
		xPathStream << m_RunControllerName << "']";
		return xPathStream.str();
	}

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerAdd document is available. It Adds
		documents to the m_DocumentMap.
	*/
	void triggered_action_add(const memory::interface::Event &event);
	void action_add(const memory::interface::Event &event);

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
	void action_execute(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerStop document is available. It stops
		the execution of a controller.
	*/
	void triggered_action_stop(const memory::interface::Event &event);

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunControllerLoadNamespace document is available. It
		creates an ObjectNamespace and sets it in m_RunController.
	*/
	void triggered_action_load_namespace(const memory::interface::Event &event);
	void action_load_namespace(const memory::interface::Event &event);

	#ifdef CBF_HAVE_QT

		/**
			@brief A thread-safe way to add an event to the event queue.
		*/
		void push_event(XCFMemoryRunController::Function func, memory::interface::Event event);

		/**
			@brief A thread-safe way to remove an event from the event queue.
		*/
		bool pop_event(std::pair<XCFMemoryRunController::Function, memory::interface::Event>* event);

	#endif

	/**
		@brief Sends (not inserts) an error XCFMemoryRunControllerNotification document.
		Whether a notification will be send defines the m_NotificationLevel.

		@param note Will be filled into the 'Note' element.
		@param documentID The dbxml:id of the xml document that triggered this note.
	*/
	void notifyError(std::string note, int documentID);
	
	/**
		@brief A notify function for the add-action.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentID The dbxml:id of the xml document that triggered this note.
		@param added_documents The a vector of added document names.
		@param ignored_documents A vector of document names that were ignored.
	*/
	void notifyAdd(int documentID, std::vector<std::string> added_documents,
					std::vector<std::string> ignored_documents);

	/**
		@brief A notify function for the load-action.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentID The dbxml:id of the xml document that triggered this note.
		@param loaded_documents The documents that could be loaded into the namespace
		@param not_found_documents A set of documents that were not found.
		@param controller_names A set of CBF::Controllers which could be loaded from the namespace.
	*/
	void notifyLoad(int documentID, std::set<std::string> loaded_documents,
			std::set<std::string> not_found_documents, std::set<std::string> controller_names);

	/**
		@brief A notify function for the stop-action.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentID The dbxml:id of the xml document that triggered this note.
		@param controller_name The name of the controller that was stopped.
	*/
	void notifyStop(int documentID, std::string controller_name);
	
	/**
		@brief A notify function for the sleep-time/steps-change.
		Whether a notification will be send defines the m_NotificationLevel.

		@param documentID The dbxml:id of the xml document that triggered this note.
		@param sleep_time The new sleep-time.
		@param steps The new amount of steps.
		@param time_set Whether the time was changed.
		@param steps_set Whether the steps were changed.
	*/
	void notifyOptions(int documentID, unsigned int sleep_time, unsigned int steps, bool time_set, bool steps_set);
	
	/**
		@brief A helper function that tries to initialize document as an CBF::Object and add it to a namespace.
		@param document An xml-document that describes a CBF::Object
		@param eventID the ID of the event that triggered this call. used for notifications.
		@return boolean whether initialization was successful.
	*/
	bool test_initializable(std::string document, int eventID);

};

/**
	@brief A shared pointer to a XCFMemoryRunController.
*/
typedef boost::shared_ptr<XCFMemoryRunController> XCFMemoryRunControllerPtr;

} //namespace
#endif

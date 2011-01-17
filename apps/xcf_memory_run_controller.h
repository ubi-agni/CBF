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

#ifdef CBF_HAVE_QT
	#include <QtGui/QApplication>
#endif

#include <Memory/Condition.hpp>
#include <Memory/Subscription.hpp>
#include <Memory/Interface.hpp>
#include <Memory/Event.hpp>

namespace CBF {

/**
	@brief A struct that runs a controller from an XCFMemoryRunController 
	document provided by an active_memory.
*/
struct XCFMemoryRunController {

	public:

	/**
		@brief This struct connects to an active_memory and subscribes for
		XCFMemoryRunController-documents with the specified controller-name.
		When the specified document ist inserted, the XCFMemoryRunController
		starts the execution of the controller.

		@param controller_name The name of the controller to listen for.
		@param active_memory_name The name of the active_memory to connect to.
		@param run_controller The CBFRunController on which the execution is performed.
				
	*/	
	XCFMemoryRunController(std::string controller_name, std::string active_memory_name,
				CBFRunControllerPtr run_controller
				);

	/**
		A virtual destructor, so polymorphic cleanup 
		works nicely
	*/
	virtual ~XCFMemoryRunController() {}

	private:

	/**
		@brief Holds the name of the controller to run.
	*/
	std::string m_ControllerName;

	/**
		@brief Holds the pointer to the MemoryInterface.
	*/
	memory::interface::MemoryInterface::pointer m_MemoryInterface;

	/**
		@brief Holds the CBFRunController which is used for the execution..
	*/
	CBFRunControllerPtr m_RunController;

	/**
		@brief Returns the string that identifies the XML-document which is listened for.
	*/
	const char* XPathString(){
		std::stringstream xPathStream;
		xPathStream << "/p1:XCFMemoryRunController[ControllerName='" << m_ControllerName << "']";
		return xPathStream.str().c_str();
	}

	/**
		@brief The function that will be called by the active_memory when 
		a XCFMemoryRunController document is available.	It sets
		the ControlBasis and starts the CBFRunController.
	*/
	void start_controller(const memory::interface::Event &event);

};
} //namespace
#endif

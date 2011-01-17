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

namespace CBF {

namespace mi = memory::interface;

		XCFMemoryRunController::XCFMemoryRunController(
				std::string controller_name,
				std::string active_memory_name,
				CBFRunControllerPtr run_controller
				)
		:
		m_ControllerName(controller_name),
		m_MemoryInterface(mi::MemoryInterface::getInstance(active_memory_name)),
		m_RunController(run_controller)
	{
		CBF_DEBUG("Subscribing at active_memory");

		mi::Condition condition(memory::interface::Event::INSERT, XPathString());
		mi::TriggeredAction triggered_action(boost::bind(&XCFMemoryRunController::start_controller, this, _1));

		mi::Subscription subscription(condition, triggered_action);

		m_MemoryInterface -> subscribe(subscription);
	}

	void XCFMemoryRunController::start_controller(const memory::interface::Event &event){
		CBF_DEBUG("doc: " << event.getDocument());
	
		std::string documentText = event.getDocument().getRootLocation().getDocumentText();
//		std::string controlBasisText = event.getDocument().getRootLocation()["ControlBasis"].getTextValue();

		CBF_DEBUG("parsing XML");
		try {
			std::istringstream s(documentText);
			std::auto_ptr<CBFSchema::XCFMemoryRunController> run_controller = 
				CBFSchema::XCFMemoryRunController_(s, xml_schema::flags::dont_validate);

			CBF::ControlBasisPtr control_basis(new CBF::ControlBasis(run_controller -> ControlBasis()));

			CBF_DEBUG("setting the control_basis");
			m_RunController -> setControlBasis(control_basis);
			m_RunController -> start_controller();

		} catch (...){
			CBF_DEBUG("Could not parse document as ControlBasis");
		}
	}


} //namespace

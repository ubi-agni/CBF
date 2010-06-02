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


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#ifndef CBF_MEMORY_RESOURCE_HH
#define CBF_MEMORY_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/resource.h>
#include <cbf/plugin_decl_macros.h>


#include <Memory/Condition.hpp>
#include <Memory/Subscription.hpp>
#include <Memory/Interface.hpp>
#include <xmltio/Document.hpp>

#include <boost/bind.hpp>

namespace CBF {


/**
	@brief A resource that can send/retrieve values over an ActiveMemory database

	uri is the URI that we expect an ActiveMemory to live at
	xpath is the XPath expression we use to listen to events
	path is the path we use to insert events
*/
struct MemoryResource : public Resource {
	MemoryResource(
		const std::string &uri, 
		const std::string &xpath,
		const std::string &path) 
	:
		m_MemoryInterface(
			memory::interface::MemoryInterface::getInstance(uri)),
		m_Subscription(
			memory::interface::Condition(0, xpath), 
			memory::interface::TriggeredAction(
				boost::bind(&MemoryResource::triggered_action, this, _1)))
	{

	}

	void triggered_action(const memory::interface::Event &ev) {

	}

	protected:
		memory::interface::MemoryInterface::pointer m_MemoryInterface;
		memory::interface::Subscription m_Subscription;
};

} // namespace

#endif

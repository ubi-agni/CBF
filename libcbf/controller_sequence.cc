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

#include <cbf/controller_sequence.h>
#include <cbf/plugin_macros.h>

#include <iostream>

namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		ControllerSequence::ControllerSequence(const ControllerSequenceType &xml_instance) {
			std::cout << "[ControllerSequence(const ControllerSequenceType &xml_instance)]: " << std::endl;
		
			//! Instantiate the subordinate controllers
			ControllerSequenceType::Controller_const_iterator it;
			for (
				it = xml_instance.Controller().begin(); 
				it != xml_instance.Controller().end();
				++it
			)
			{
					ControllerPtr controller(PluginPool<Controller>::get_instance()->create_from_xml(*it));
					m_Controllers.push_back(controller);
			}
		
			m_Iterator = m_Controllers.begin();
		}
	#endif
	
	bool ControllerSequence::step() {
		if (finished() == true)
			m_Iterator = m_Controllers.begin();
	
		if (m_Iterator != m_Controllers.end())
			(*m_Iterator)->step();
	
		if ((*m_Iterator)->finished() == true)
			++m_Iterator;
	
		return finished();
	}
	
	bool ControllerSequence::finished() {
		if (m_Iterator == m_Controllers.end())
			return true;
	
		return false;
	}
	
	CBF_PLUGIN_CLASS(ControllerSequence, Controller)
	
} // namespace


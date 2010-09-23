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
#include <iostream>

#ifdef CBF_HAVE_XSD
	#include <cbf/xml_factory.h>
#endif

namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		ControllerSequence::ControllerSequence(const CBFSchema::ControllerSequence &xml_instance) {
			std::cout << "[ControllerSequence(const ControllerSequenceType &xml_instance)]: " << std::endl;
		
			//! Instantiate the subordinate controllers
			CBFSchema::ControllerSequence::Controller1_const_iterator it;
			for (
				it = xml_instance.Controller1().begin(); 
				it != xml_instance.Controller1().end();
				++it
			)
			{
					ControllerPtr controller(XMLBaseFactory<Controller, CBFSchema::Controller>::instance()->create(*it));
					m_Controllers.push_back(controller);
			}
		
			m_Iterator = m_Controllers.begin();
		}

		static XMLDerivedFactory<
			ControllerSequence, 
			CBFSchema::ControllerSequence, 
			Controller, 
			CBFSchema::Controller> 
		x1;

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
} // namespace


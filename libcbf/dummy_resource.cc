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

#include <cbf/dummy_resource.h>
#include <cbf/debug_macros.h>
#include <cbf/plugin_macros.h>
#include <cbf/utilities.h>
#include <cbf/xml_factories.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <deque>

#include <boost/numeric/ublas/io.hpp>


namespace CBF {
	void DummyResource::add(const FloatVector &arg) {
		set(m_Variables + arg);
		CBF_DEBUG("current values" << m_Variables);
	}


	#ifdef CBF_HAVE_XSD
		DummyResource::DummyResource(const ::DummyResource &xml_instance)
		{
			m_Variables = create_vector(xml_instance.Vector());
			CBF_DEBUG("current values: " << m_Variables)
		
		}
	#endif
	
	CBF_PLUGIN_CLASS(DummyResource, Resource)
} //namespace


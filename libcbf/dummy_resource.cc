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
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>
#include <cbf/foreign_object.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <deque>

namespace CBF {
	void DummyResource::add(const FloatVector &arg) {
		m_Variables += arg;
		CBF_DEBUG("current values" << m_Variables.transpose());
	}


	#ifdef CBF_HAVE_XSD
		DummyResource::DummyResource(
			const CBFSchema::DummyResource &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			Resource(xml_instance, object_namespace)
		{
			m_Variables = 
				*XMLFactory<FloatVector>::instance()->create(
					xml_instance.Vector(), object_namespace
				)
			;

			CBF_DEBUG("current values: " << m_Variables);
		
		}

		static XMLDerivedFactory<DummyResource, CBFSchema::DummyResource> x;
	#endif
	
} //namespace


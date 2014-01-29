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

/* -*- mode: c-non-suck; -*- */

#include <cbf/spacenavi_reference.h>
#include <cbf/xml_object_factory.h>
#include <spacenavi.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
SpaceNaviReference::SpaceNaviReference(
	const CBFSchema::SpaceNaviReference &xml_instance,
	ObjectNamespacePtr object_namespace) : 
	Reference(xml_instance, object_namespace) 
{
	init();
}

void SpaceNaviReference::init() 
{
	m_References.push_back(FloatVector(6));
	m_Device = snavi_open(NULL, O_NONBLOCK);
	if (m_Device == 0)
		throw std::runtime_error("Could not open SpaceMouse device"); 
	// turn on LED
	snavi_set_led (m_Device, 1);

	m_References.resize(1);
	m_References[0] = FloatVector(6);
}

SpaceNaviReference::~SpaceNaviReference() 
{
	if (m_Device) {
		snavi_set_led (m_Device, 0);
		snavi_close (m_Device);
	}
}
void SpaceNaviReference::update() 
{
	snavi_event_t e;
	/** get all events from the queue */
	while(snavi_get_event(m_Device, &e) >= 0) {
		if (e.type == MotionEvent) {
			FloatVector tmp;
			// TODO accumulate axes values
			tmp << e.axes[0], e.axes[1], e.axes[2], e.axes[3], e.axes[4], e.axes[5];
			m_References[0] = tmp;
		}
	}
}

static XMLDerivedFactory<SpaceNaviReference, CBFSchema::SpaceNaviReference> x;

#endif

} // namespace






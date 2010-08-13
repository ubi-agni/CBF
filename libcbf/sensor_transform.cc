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

#include <cbf/sensor_transform.h>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

#include <iostream>

namespace CBF {

#ifdef CBF_HAVE_XSD
SensorTransform::SensorTransform(const SensorTransformType &xml_instance) {
	for (
		SensorTransformType::ComponentName_sequence::const_iterator it = xml_instance.ComponentName().begin();
		it != xml_instance.ComponentName().end();
		++it) {
			m_ComponentNames.push_back(*it);
	}
}

template<> 
	XMLBaseFactory<SensorTransform, SensorTransformType>* 
		XMLBaseFactory<SensorTransform, SensorTransformType>::m_Instance = 0;


#endif

} // namespace


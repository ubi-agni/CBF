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
#include <cbf/xml_object_factory.h>
#include <cbf/foreign_object.h>
#include <cbf/xml_factory.h>
#include <cbf/utilities.h>

#include <iostream>

namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		SensorTransform::SensorTransform(
			const CBFSchema::SensorTransform &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			Object(xml_instance, object_namespace)
		{
			for (
				CBFSchema::SensorTransform::ComponentName_sequence::const_iterator it 
					= xml_instance.ComponentName().begin();
				it != xml_instance.ComponentName().end();
				++it) 
			{
				m_ComponentNames.push_back(*it);
			}
		}

		ConstantSensorTransform::ConstantSensorTransform(
			const CBFSchema::ConstantSensorTransform &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			SensorTransform(xml_instance, object_namespace)	
		{
			init(*XMLFactory<FloatVector>::instance()->create(xml_instance.Value(), object_namespace));
		}

		BlockWiseMultiplySensorTransform::BlockWiseMultiplySensorTransform(
			const CBFSchema::BlockWiseMultiplySensorTransform &xml_instance,
			ObjectNamespacePtr object_namespace
		) :
			SensorTransform(xml_instance, object_namespace)
		{
			init(
				XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace),
				xml_instance.Blocksize(),
				*XMLFactory<FloatVector>::instance()->create(xml_instance.Factors(), object_namespace)
			);
		}

		static XMLDerivedFactory<BlockWiseMultiplySensorTransform, CBFSchema::BlockWiseMultiplySensorTransform> x;

	#endif

} // namespace


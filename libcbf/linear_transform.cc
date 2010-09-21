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

#include <cbf/linear_transform.h>
#include <cbf/utilities.h>
#include <cbf/xml_factories.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD
		LinearSensorTransform::LinearSensorTransform(const CBFSchema::LinearSensorTransform &xml_instance) 
		{
			CBF_DEBUG("[LinearSensorTransform(const LinearSensorTransformType &xml_instance)]: yay!!!");

			m_TaskJacobian = create_matrix(xml_instance.Matrix());
		}

		static XMLDerivedFactory<LinearSensorTransform, CBFSchema::LinearSensorTransform, SensorTransform, CBFSchema::SensorTransform> x;
	#endif
		
} // namespace

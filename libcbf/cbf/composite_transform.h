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

#ifndef CBF_COMPOSITE_TRANSFORM_HH
#define CBF_COMPOSITE_TRANSFORM_HH

#include <cbf/sensor_transform.h>

#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

namespace CBFSchema { class CompositeSensorTransform; }

namespace CBF {
	
	namespace ublas = boost::numeric::ublas;
	
	/**
		@brief This sensor transform allows to compose different sensor transforms into a single
		one. 
	
		If T1 and T2 are sensor transforms transforming to task space variables t1 and t2 
		respectively, then this sensor transform transforms to (t1' t2')', i.e. the column 
		vector composed of the components of t1 and t2.
	
		This allows e.g. composition of a position/orientation controller from a position
		and an orientation controller.

		Note that you are not limited to two transforms. But you can use an arbitrary
		positive non zero number of transforms...
	*/
	struct CompositeSensorTransform : public SensorTransform {

		protected:
			std::vector<SensorTransformPtr> m_SensorTransforms;
	
		public:
			CompositeSensorTransform(std::vector<SensorTransformPtr> transforms = std::vector<SensorTransformPtr>()) 
			{
				set_transforms(transforms);
			}

			CompositeSensorTransform(const CBFSchema::CompositeSensorTransform &xml_instance);

			CompositeSensorTransform(SensorTransformPtr t1, SensorTransformPtr t2)
			{
				std::vector<SensorTransformPtr> v;
				v.push_back(t1);
				v.push_back(t2);
				set_transforms(v);
			}

			virtual void set_transforms(std::vector<SensorTransformPtr> transforms);
		
			virtual void update(const FloatVector &resource_value);
	
			virtual const std::vector<SensorTransformPtr> &transforms() const {
				return m_SensorTransforms;
			}
	};
	
	typedef boost::shared_ptr<CompositeSensorTransform> CompositeSensorTransformPtr;

	/**
		This Sensor Transform allows to view a SensorTransform with a large
		task space as a CompositeSensorTransform allowing to apply a
		UnarySensorTransformOperation to each block..
	*/
	struct AsCompositeSensorTransform : public SensorTransform {
		//! The transform we are wrapping
		SensorTransformPtr m_Transform;

		
	};
	
} // Namespace

#endif

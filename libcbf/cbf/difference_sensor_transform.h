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

#ifndef CBF_DIFFERENCE_SENSOR_TRANSFORM
#define CBF_DIFFERENCE_SENSOR_TRANSFORM

#include <cbf/sensor_transform.h>
#include <cbf/namespace.h>

#include <vector>
#include <cassert>

namespace CBFSchema { class DifferenceSensorTransform; }

namespace CBF {

	/**
		@brief: A SensorTransform to calculate differences of two given SensorTransforms 
		operating on the same resources..
	*/
	struct DifferenceSensorTransform : public SensorTransform {
		DifferenceSensorTransform(const CBFSchema::DifferenceSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

		DifferenceSensorTransform(SensorTransformPtr t1 = SensorTransformPtr(), SensorTransformPtr t2 = SensorTransformPtr()) 
		{
			if (t1.get() && t2.get())
				set_transforms(t1, t2);
		}

		void set_transforms(SensorTransformPtr t1, SensorTransformPtr t2) {
			m_Transform1 = t1;
			m_Transform2 = t2;
		}

		void update(const FloatVector &resource_value) {
			assert(m_Transform1->task_jacobian().size1() == m_Transform2->task_jacobian().size1());

			m_Transform1->update(resource_value);
			m_Transform2->update(resource_value);

			//! The jacobian is just the difference of the individual transforms
			m_TaskJacobian = m_Transform1->task_jacobian() - m_Transform2->task_jacobian();

			//! And the result is just the difference of the individual results.
			m_Result = m_Transform1->result() - m_Transform2->result();
		}

		unsigned int resource_dim() const { return m_Transform1->resource_dim(); }

		// As both sensor transforms are required to have the same task dimensionality, it does not matter
		// which one we return
		unsigned int task_dim() const { return m_Transform1->task_dim(); }

		protected:
			SensorTransformPtr m_Transform1;
			SensorTransformPtr m_Transform2;
	};

	typedef boost::shared_ptr<DifferenceSensorTransform>
		DifferenceSensorTransformPtr;
	
} // namespace

#endif

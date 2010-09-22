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

#ifndef CBF_LINEAR_TRANSFORM_HH
#define CBF_LINEAR_TRANSFORM_HH

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/utilities.h>

namespace CBFSchema { class LinearSensorTransform; }

namespace CBF {

class LinearSensorTransform;
typedef boost::shared_ptr<LinearSensorTransform> LinearSensorTransformPtr;

/**
	@brief A linear sensor transform
*/
struct LinearSensorTransform : public SensorTransform {
	LinearSensorTransform (const CBFSchema::LinearSensorTransform &xml_instance);

	void update() {
		m_Result = ublas::prod(m_TaskJacobian, m_Resource->get());
	}

	LinearSensorTransform() {
		m_TaskJacobian = ublas::identity_matrix<Float>(1);
	}

	LinearSensorTransform(FloatMatrix &m) {
		m_TaskJacobian = m ;
	}

	/**
		Needs to be implemented in subclass to allow dimensionality checking when
		this is bound to a resource.
	*/
	virtual unsigned int resource_dim() const {
		return m_TaskJacobian.size2();
	}

	/**
		Needs to be implemented in subclass to allow dimensionality checking when
		this is bound to a resource.
	*/
	virtual unsigned int task_dim() const {
		return m_TaskJacobian.size1();
	}
};


} // namespace

#endif

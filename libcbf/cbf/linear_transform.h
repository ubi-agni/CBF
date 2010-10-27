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

	void update(const FloatVector &resource_value) {
		m_Result = ublas::prod(m_CoefficientMatrix, resource_value);
	}

	LinearSensorTransform(const FloatMatrix &coefficient_matrix) 
	{
		init(coefficient_matrix);
	}

	void init (const FloatMatrix &coefficient_matrix) {
		m_CoefficientMatrix = coefficient_matrix;
		m_TaskJacobian = ublas::identity_matrix<Float>(
			coefficient_matrix.size1(), 
			coefficient_matrix.size2()
		);
	}

	LinearSensorTransform(FloatMatrix &m) 
	{
		m_TaskJacobian = m ;
	}

	/** 
		A linear map is described by a matrix of coefficients
	*/
	FloatMatrix m_CoefficientMatrix;
};


} // namespace

#endif

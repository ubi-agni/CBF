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
#include <cbf/plugin_macros.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD
		LinearSensorTransform::LinearSensorTransform(const LinearSensorTransformType &xml_instance) 
		{
			CBF_DEBUG("[LinearSensorTransform(const LinearSensorTransformType &xml_instance)]: yay!!!");
			const MatrixType *m = &xml_instance.Matrix();
		
			const BoostMatrixType *m2 = 
				dynamic_cast<const BoostMatrixType*>(m);

			if (m2) {
				std::stringstream stream(std::string(m2->String()));
				stream >> m_Matrix;
				CBF_DEBUG(m_Matrix)
				if ((m_Matrix.size1() == 0) && (m_Matrix.size2() == 0)) 
					throw std::runtime_error
						("[LinearSensorTransform]: Matrix is empty");
			}
		}
	#endif
		
	CBF_PLUGIN_CLASS(LinearSensorTransform, SensorTransform)
} // namespace

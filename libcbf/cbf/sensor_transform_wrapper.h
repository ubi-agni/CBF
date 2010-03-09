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

#ifndef CBF_SENSOR_TRANSFORM_WRAPPER_HH
#define CBF_SENSOR_TRANSFORM_WRAPPER_HH

#include <vector>

#include <cbf/sensor_transform.h>

namespace CBF {

	/** @brief 
			This SensorTransform can be used to select a number
			of rows of a jacobian
	*/
	struct RowSelector : public SensorTransform {
		/** The SensorTransform we are wrapping */
		SensorTransformPtr m_SensorTransform;

		/** The rows we want to keep */
		std::vector<int> m_Rows;

		std::vector<int> &rows() { return m_Rows; }

		void set_rows(const std::vector<int> &rows) { m_Rows = rows; }

		virtual void read() {
			m_SensorTransform->read();
		}
	};

} // namespace

#endif

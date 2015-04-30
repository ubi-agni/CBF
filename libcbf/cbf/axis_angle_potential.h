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

#ifndef CBF_AXIS_ANGLE_ORIENTATION_POTENTIALS_HH
#define CBF_AXIS_ANGLE_ORIENTATION_POTENTIALS_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class AxisAnglePotential; }

namespace CBF {
	
	/**
		@brief A potential function operating on the space of rotations...
	
		represented as Axis Angle, but with all information encoded into the
		direction and length of the axis.
	*/
	struct AxisAnglePotential : public Potential {
		AxisAnglePotential(const CBFSchema::AxisAnglePotential &xml_instance, ObjectNamespacePtr object_namespace);

		AxisAnglePotential() : Potential()
		{
			m_CurrentReference = FloatVector(sensor_dim());
		}

		virtual FloatVector &select_reference(
			const std::vector<FloatVector > &references,
			const FloatVector &input
		);

		void gradient (
			FloatVector &result,
			const FloatVector &reference,
			const FloatVector &input
		);

		virtual void integration (
			FloatVector &nextpos,
			const FloatVector &currentpos,
			const FloatVector &currentvel,
			const Float timestep
		);

		virtual unsigned int sensor_dim() const { return 3u; }

		virtual unsigned int task_dim() const { return 3u; }

		virtual Float distance(const FloatVector &v1, const FloatVector &v2);

		virtual Float norm(const FloatVector &v) {
			return normalize_angle(v.norm());
		}
	};

	typedef boost::shared_ptr<AxisAnglePotential> AxisAnglePotentialPtr;

} // namespace

#endif



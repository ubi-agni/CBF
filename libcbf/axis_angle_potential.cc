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

#include <cbf/axis_angle_potential.h>
#include <cbf/quaternion.h>
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

		Float AxisAnglePotential::distance(const FloatVector &v1, const FloatVector &v2) {

			Quaternion q1; q1.from_axis_angle3(v1);
			Quaternion q2; q2.from_axis_angle3(v2);
			if (q1.dot(q2) < 0) q2 *= -1.;

			Quaternion q3 = q2 * q1.conjugate();
			Float angle = normalize_angle(acos(q3.w) * 2.0);
			angle = fabs(angle);

			CBF_DEBUG("angle [distance]: " << angle);
			return angle;
		}

		FloatVector &AxisAnglePotential::select_reference(
			const std::vector<FloatVector > &references,
			const FloatVector &input)
		{
			assert(references.size() > 0);

			//! Find the closest reference
			Float min_distance = distance(references[0], input);
			unsigned int min_index = 0;

			for (unsigned int i = 1; i < references.size(); ++i) {
				Float cur_distance = distance(references[i], input);
				
				if (cur_distance < min_distance) {
					min_index = i;
					min_distance = cur_distance;
				}
			}

			m_CurrentReference = references[min_distance];

			return m_CurrentReference;
		}

		void AxisAnglePotential::gradient (
			FloatVector &result,
			const FloatVector &reference,
			const FloatVector &input)
		{

			CBF_DEBUG("[AxisAnglePotential]: input: " << input);
			CBF_DEBUG("[AxisAnglePotential]: ref: " << m_CurrentReference);
			Quaternion in;
			in.from_axis_angle3(input);
			CBF_DEBUG("q_in: " << in);

			Quaternion ref;
			ref.from_axis_angle3(m_CurrentReference);
			CBF_DEBUG("q_ref: " << ref);

			Quaternion step = qslerp(in, ref, 1.0);
			CBF_DEBUG("step: " << step);

			Quaternion res = step  * in.conjugate();
			CBF_DEBUG("res: " << res);

			result.resize(3);
			res.to_axis_angle3(result);
			CBF_DEBUG("result: " << result);
		}

    void AxisAnglePotential::integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep)
    {
      nextpos = currentpos+ taskvel*timestep;
    }

	#ifdef CBF_HAVE_XSD
		AxisAnglePotential::AxisAnglePotential(const CBFSchema::AxisAnglePotential &xml_instance, ObjectNamespacePtr object_namespace) :
			Potential(xml_instance, object_namespace) {
			CBF_DEBUG("[AxisAnglePotential(const AxisAnglePotentialType &xml_instance)]: yay!");

		}

		static XMLDerivedFactory<AxisAnglePotential, CBFSchema::AxisAnglePotential> x;
	#endif
} // namespace

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


<<<<<<< HEAD
		void AxisAnglePotential::gradient (
			FloatVector &result,
			const std::vector<FloatVector > &references,
			const FloatVector &input
		) {
			CBF_DEBUG("[AxisAnglePotential]: input: " << input.transpose());
			CBF_DEBUG("[AxisAnglePotential]: ref: " << references[0].transpose());
			Quaternion in; in.from_axis_angle3(input);
			Quaternion ref; ref.from_axis_angle3(references[0]);
			if (in.dot(ref) < 0) ref *= -1.;
=======
    void AxisAnglePotential::gradient (
      FloatVector &result,
      const std::vector<FloatVector > &references,
      const FloatVector &input)
    {

			CBF_DEBUG("[AxisAnglePotential]: input: " << input);
			CBF_DEBUG("[AxisAnglePotential]: ref: " << references[0]);
			Quaternion in;
			in.from_axis_angle3(input);
			CBF_DEBUG("q_in: " << in);

			Quaternion ref;
			ref.from_axis_angle3(references[0]);
			CBF_DEBUG("q_ref: " << ref);

      Quaternion step = qslerp(in, ref, 1.0);
			CBF_DEBUG("step: " << step);
>>>>>>> 8315659... added task_space_planner

			Quaternion step = ref * in.conjugate();
			CBF_DEBUG("q_step: " << step);

			result.resize(3);
			step.to_axis_angle3(result);
			result *= m_Coefficient;
			Float result_norm = result.norm();

<<<<<<< HEAD
			// Normalize gradient step so it's not bigger than m_MaxGradientStep
			if (result_norm >= m_MaxGradientStepNorm)
				result *= m_MaxGradientStepNorm/result_norm;
=======
//			if(norm(result) > m_MaxGradientStepNorm)
//				result *= m_MaxGradientStepNorm/norm(result);
>>>>>>> 8315659... added task_space_planner

			CBF_DEBUG("result: " << result.transpose());
		}

    void AxisAnglePotential::integration (FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep)
    {
      nextpos = currentpos+ 0.5*taskvel*timestep;
    }

	#ifdef CBF_HAVE_XSD
		AxisAnglePotential::AxisAnglePotential(const CBFSchema::AxisAnglePotential &xml_instance, ObjectNamespacePtr object_namespace) :
			Potential(xml_instance, object_namespace) {
			CBF_DEBUG("[AxisAnglePotential(const AxisAnglePotentialType &xml_instance)]: yay!");
			CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());
		}

		static XMLDerivedFactory<AxisAnglePotential, CBFSchema::AxisAnglePotential> x;
	#endif
} // namespace

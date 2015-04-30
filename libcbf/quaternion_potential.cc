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
/*
 * quaternion_potential.cc
 *
 * This code is implemented based on [1]
 *  [1] A. Ude et al. "Orientation in Cartesian space ...", ICRA, 2014
 *
 *  Created on April. 21, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/quaternion_potential.h>
#include <cbf/quaternion.h>
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

  Float QuaternionPotential::distance(const FloatVector &v1, const FloatVector &v2)
  {
    Quaternion q3 = Quaternion(v1) * Quaternion(v2).conjugate();

    Float angle = normalize_angle(acos(q3.w) * 2.0);

    return fabs(angle);
  }

  FloatVector &QuaternionPotential::select_reference(
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

  // result : angular velocity, in R^3
  // input, reference  : quaternion , in R^4 (w, x, y, z)
  void QuaternionPotential::gradient (
      FloatVector &result,
      const FloatVector &reference,
      const FloatVector &input)
  {
    Quaternion res;
    res = Quaternion(reference)  * Quaternion(input).conjugate();

    result(0) = res.x;
    result(1) = res.y;
    result(2) = res.z;

    if (result.norm() < CBF_QUAT_AXIS_THRESH) {
      result.setZero();
    }
    else {
      result = 2.0* acos(res.w) * result/ result.norm();
    }
  }

  // nextpos    : quaternion at t + timestep, in R^4
  // currentpos : quaternion at t, in R^4
  // currentvel : angular velocity at t, in R^3
  // timestep   : delta t, in sec
  void QuaternionPotential::integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep)
  {
    Quaternion res;
    FloatVector res_axis(3);

    Float lwnorm = taskvel.norm()*timestep/2.0;

    if (lwnorm < CBF_QUAT_AXIS_THRESH) {
      nextpos = currentpos;
    }
    else {
      res.w = cos(lwnorm);
      res_axis =  sin(lwnorm)*taskvel*timestep/2.0/lwnorm;

      res.x = res_axis(0);
      res.y = res_axis(1);
      res.z = res_axis(2);

      nextpos = res*Quaternion(currentpos);
    }

  }


	#ifdef CBF_HAVE_XSD
    QuaternionPotential::QuaternionPotential(const CBFSchema::QuaternionPotential &xml_instance, ObjectNamespacePtr object_namespace) :
			Potential(xml_instance, object_namespace) {

    }

    static XMLDerivedFactory<QuaternionPotential, CBFSchema::QuaternionPotential> x;
	#endif

} // namespace

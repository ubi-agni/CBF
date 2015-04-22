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

#include <cbf/direction_potential.h>
#include <cbf/xml_object_factory.h>

#include <Eigen/Dense>


namespace CBF {


  void DirectionPotential::gradient (
    FloatVector &result,
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

    if( min_distance < CBF_DIRECTION_DIFFERENCE_THRESHOLD) {
      result.setZero();
    }
    else {
      result = Eigen::Vector3d(input).cross(Eigen::Vector3d(references[min_index])).cross(Eigen::Vector3d(input));
      result = result/result.norm()*min_distance;
    }

  }

  void DirectionPotential::integration (
      FloatVector &nextpos,
      const FloatVector &currentpos,
      const FloatVector &currentvel,
      const Float timestep)
  {
    nextpos = currentpos + 0.5*currentvel*timestep;
    nextpos = nextpos/nextpos.norm();
  }

  Float DirectionPotential::distance(const FloatVector &v1, const FloatVector &v2)
  {
    return acos(v1.normalized().dot(v2.normalized()));
  }

  Float DirectionPotential::norm(const FloatVector &v)
  {
    return v.norm();
  }


	#ifdef CBF_HAVE_XSD
//    DirectionPotential::DirectionPotential(const CBFSchema::DirectionPotential &xml_instance, ObjectNamespacePtr object_namespace) :
//			Potential(xml_instance, object_namespace) {

//		}

//		static XMLDerivedFactory<
//      DirectionPotential,
//			CBFSchema::AxisPotential
//		> x1;
	#endif

} // namespace

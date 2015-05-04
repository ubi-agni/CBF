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

#include <cbf/axis_potential.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

  FloatVector &AxisPotential::select_reference(
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

    m_CurrentReference = references[min_index];

    return m_CurrentReference;
  }

  void AxisPotential::gradient (
      FloatVector &result,
      const FloatVector &reference,
      const FloatVector &input)
  {
    //! Now that we have the reference closest to our input, calculate
    //! SLERP step into that direction

    FloatVector ref = (1.0/(reference).norm()) * reference;

    FloatVector in = (1.0/input.norm())
      * input;

    FloatVector s(sensor_dim());
    slerp(in, ref, 1.0, s);

    result = s - in;
  }

  void AxisPotential::integration (
      FloatVector &nextpos,
      const FloatVector &currentpos,
      const FloatVector &taskvel,
      const Float timestep)
  {
    nextpos = currentpos + taskvel*timestep;
    nextpos = nextpos/nextpos.norm();
  }

	#ifdef CBF_HAVE_XSD

		AxisPotential::AxisPotential(const CBFSchema::AxisPotential &xml_instance, ObjectNamespacePtr object_namespace) :
			Potential(xml_instance, object_namespace) {
			CBF_DEBUG("[AxisAnglePotential(const AxisAnglePotentialType &xml_instance)]: yay!");

		}

		static XMLDerivedFactory<
			AxisPotential, 
			CBFSchema::AxisPotential
		> x1;

	#endif
} // namespace

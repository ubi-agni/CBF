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

#ifndef CBF_AXIS_ORIENTATION_POTENTIALS_HH
#define CBF_AXIS_ORIENTATION_POTENTIALS_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/quaternion.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class AxisPotential; }

namespace CBF {

	/**
		@brief A potential that only cares about the direction of an axis
		and does n-dimensional SLERP to calculate an update step..
	*/
  struct AxisPotential : public Potential {
    AxisPotential(const CBFSchema::AxisPotential &xml_instance, ObjectNamespacePtr object_namespace);


    AxisPotential() :
      Potential()
		{

		}

		virtual void gradient (
			FloatVector &result, 
			const std::vector<FloatVector > &references, 
      const FloatVector &input
    ) {
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

      //! Now that we have the reference closest to our input, calculate
      //! SLERP step into that direction

      FloatVector ref = (1.0/(references[min_index]).norm())
        * references[min_index];

      FloatVector in = (1.0/input.norm())
        * input;

      FloatVector s(sensor_dim());
      slerp(in, ref, 1.0, s);

      result = s - in;
    }
	
    virtual void integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &currentvel,
        const Float timestep
    );

    virtual unsigned int sensor_dim() const { return 3u; }

    virtual unsigned int task_dim() const { return 3u; }

		/** 
			@brief This "distance" is the remaining angle between v1 and v2 
		 */
		virtual Float distance(const FloatVector &v1, const FloatVector &v2) {
			return acos(v1.normalized().dot(v2.normalized()));
		}

		virtual Float norm(const FloatVector &v) {
			CBF_THROW_RUNTIME_ERROR("No sensible definition of norm possible on this space");
			return v.norm();
		}
	};

  typedef boost::shared_ptr<AxisPotential> AxisPotentialPtr;

} // namespace

#endif



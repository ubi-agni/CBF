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

#ifndef CBF_DIRECTION_POTENTIAL_H
#define CBF_DIRECTION_POTENTIAL_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/quaternion.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

#define CBF_DIRECTION_DIFFERENCE_THRESHOLD 0.000001

namespace CBFSchema { class DirectionPotential; }

namespace CBF {

  /**
    @brief A potential that only cares about the direction of an axis
    and does n-dimensional SLERP to calculate an update step..
  */
  struct DirectionPotential : public Potential {

    DirectionPotential(const CBFSchema::DirectionPotential &xml_instance, ObjectNamespacePtr object_namespace);

    DirectionPotential() :
      Potential()
    {

    }

    virtual void gradient (
      FloatVector &result,
      const std::vector<FloatVector > &references,
      const FloatVector &input
    );

    virtual void integration (FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep
    );

    virtual unsigned int sensor_dim() const { return 3u; }

    virtual unsigned int task_dim() const { return 3u; }

    virtual Float distance(const FloatVector &v1, const FloatVector &v2);

    virtual Float norm(const FloatVector &v);
  };

  typedef boost::shared_ptr<DirectionPotential> DirectionPotentialPtr;

} // namespace

#endif //CBF_DIRECTION_POTENTIAL_H



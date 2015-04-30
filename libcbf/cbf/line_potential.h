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
 * line_potential.h
 *
 * It controls the position of the end-effector to be on a line that is consisted
 * with a "reference" point and line direction
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef LINE_POTENTIAL_H
#define LINE_POTENTIAL_H

#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/config.h>
#include <cbf/namespace.h>
#include <kdl/frames.hpp>

#include <boost/shared_ptr.hpp>

namespace CBF {

struct LinePotential : public Potential {
  FloatVector m_LineDirection;

  LinePotential(const LinePotential &xml_instance, ObjectNamespacePtr object_namespace);
  LinePotential();

  virtual Float norm(const FloatVector &v);
  virtual Float distance(const FloatVector &v1, const FloatVector &v2);

  virtual unsigned int sensor_dim() const { return 3u; }

  virtual unsigned int task_dim() const { return 3u; }

  virtual FloatVector &select_reference(
      const std::vector<FloatVector > &references,
      const FloatVector &input
  );

  virtual void gradient (
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

  void setLineDirection(const FloatVector &v);
  void setInputVelocity(const FloatVector &Velocity);
};

typedef boost::shared_ptr<LinePotential> LinePotentialPtr;

} // CBF


#endif // LINE_POTENTIAL_H

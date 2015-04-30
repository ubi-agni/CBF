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
 * line_potential.cc
 *
 * It controls the position of the end-effector to be on a line that is consisted
 * with a "reference" point and line direction
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include "cbf/line_potential.h"

#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

#include <Eigen/Dense>

namespace CBF {

LinePotential::LinePotential()
{
  m_LineDirection = FloatVector(3);
  m_LineDirection(1) = 1.0;

  m_CurrentReference = FloatVector::Zero(sensor_dim());
}

Float LinePotential::norm(const FloatVector &v)
{
  return v.norm();
}

Float LinePotential::distance(const FloatVector &v1, const FloatVector &v2)
{
  Eigen::Vector3d x0,x1,x2;

  for(int i=0; i<3; i++)
  {
    x0(i) = v1(i);
    x1(i) = v2(i);
    x2(i) = v2(i)+m_LineDirection(i);
  }

  return ((x0-x1).cross(x0-x2)).norm()/(x2-x1).norm();
}

FloatVector &LinePotential::select_reference(
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

void LinePotential::gradient (
    FloatVector &result,
    const FloatVector &reference,
    const FloatVector &input)
{
  // find a normal vector from the input vector to the line
  FloatVector lProjectionVec(3);
  FloatVector lReference(3);

  lProjectionVec = input- reference;
  lProjectionVec = m_LineDirection* m_LineDirection.dot(lProjectionVec);

  lReference = reference + lProjectionVec;

  result  = (lReference - input);
}

void LinePotential::integration (
    FloatVector &nextpos,
    const FloatVector &currentpos,
    const FloatVector &taskvel,
    const Float timestep)
{
  nextpos = currentpos + taskvel*timestep;
}


void LinePotential::setLineDirection(const FloatVector &v)
{
  m_LineDirection = v;
  m_LineDirection.normalize();
}

void LinePotential::setInputVelocity(const FloatVector &Velocity)
{

}

} // namespace

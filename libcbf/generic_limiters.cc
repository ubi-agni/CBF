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
 * generic_limiters.cc
 *
 * generic limiters
 *
 *  Created on May 13, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/generic_limiters.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

  DampedResourceLimiter::DampedResourceLimiter(const Float timestep, const unsigned int dim) :
    Limiter(timestep, dim, dim) {

    m_PosMargin = FloatVector::Zero(dim);
    m_VelMargin = FloatVector::Zero(dim);
  }

  void DampedResourceLimiter::limit(
      const FloatVector &current_pos,
      const FloatVector &current_vel,
      FloatVector &next_pos,
      FloatVector &next_vel) {

    for (unsigned int i=0; i<m_Dim; i++) {

      if (next_vel(i) > m_VelLimit(i)) {
        next_vel(i) = m_VelLimit(i);
        next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
      }
      else if (next_vel(i) < -m_VelLimit(i)) {
        next_vel(i) = -m_VelLimit(i);
        next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
      }

      // upper pos limit
      if (next_pos(i) > m_PosUpperLimit(i)) {
        next_pos(i) = m_PosUpperLimit(i);
        next_vel(i) = (next_pos(i)-current_pos(i))/m_TimeStep;
      }
      else if (next_pos(i) > m_PosUpperLimit(i)-m_PosMargin(i)) {
        next_vel(i) *= (m_PosUpperLimit(i)-next_pos(i))/m_PosMargin(i);
        next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
      }

      // lower pos limit
      if (next_pos(i) < m_PosLowerLimit(i)) {
        next_pos(i) = m_PosLowerLimit(i);
        next_vel(i) = (next_pos(i)-current_pos(i))/m_TimeStep;
      }
      else if (next_pos(i) < m_PosLowerLimit(i)+m_PosMargin(i)) {
        next_vel(i) *= (next_pos(i)-m_PosLowerLimit(i))/m_PosMargin(i);
        next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
      }

    }
  }

  void DampedResourceLimiter::setMargin(const FloatVector pos_margin, const FloatVector vel_margin)
  {
    m_PosMargin = pos_margin;
    m_VelMargin = vel_margin;
  }


#ifdef CBF_HAVE_XSD

#endif

} // namespace

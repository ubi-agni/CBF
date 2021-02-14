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
    m_PosMargin = FloatVector::Ones(dim)*0.01;
  }

  void DampedResourceLimiter::limit(const FloatVector &current_pos, const FloatVector &current_vel,
                                    FloatVector &next_pos, FloatVector &next_vel) {

    for (unsigned int i=0; i<current_pos.size(); i++) {
      // move back into bounds if neccessary
      if (current_pos(i) > m_PosUpperLimit(i)) {
        next_vel(i) = std::max(-m_VelLimit(i), (m_PosUpperLimit(i)-current_pos(i))/m_TimeStep);
        goto UPDATE_POS;
      } else if (current_pos(i) < m_PosLowerLimit(i)) {
        next_vel(i) = std::min( m_VelLimit(i), (m_PosLowerLimit(i)-current_pos(i))/m_TimeStep);
        goto UPDATE_POS;
      }

      /*** current_pos is within bounds ***/
      // limit velocity
      if (next_vel(i) > m_VelLimit(i)) {
        next_vel(i) = m_VelLimit(i);
        next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
      } else if (next_vel(i) < -m_VelLimit(i)) {
        next_vel(i) = -m_VelLimit(i);
        next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
      }

      // upper pos limit
      if (next_pos(i) > m_PosUpperLimit(i)) {
        next_vel(i) = (m_PosUpperLimit(i)-current_pos(i))/m_TimeStep;
        goto UPDATE_POS;
      } else if (next_pos(i) > (m_PosUpperLimit(i)-m_PosMargin(i)) && next_vel(i) > 0) {
        next_vel(i) *= (m_PosUpperLimit(i)-next_pos(i))/m_PosMargin(i);
        goto UPDATE_POS;
      }

      // lower pos limit
      if (next_pos(i) < m_PosLowerLimit(i)) {
        next_vel(i) = (m_PosLowerLimit(i)-current_pos(i))/m_TimeStep;
        goto UPDATE_POS;
      } else if (next_pos(i) < (m_PosLowerLimit(i)+m_PosMargin(i)) && next_vel(i) < 0) {
        next_vel(i) *= (next_pos(i)-m_PosLowerLimit(i))/m_PosMargin(i);
        goto UPDATE_POS;
      }

      continue;

UPDATE_POS:  // finally update next_pos
      next_pos(i) = current_pos(i) + next_vel(i)*m_TimeStep;
    }
  }

  void DampedResourceLimiter::setMargin(const FloatVector& pos_margin)
  {
    assert(m_PosMargin.size() == pos_margin.size());
    m_PosMargin = pos_margin;
  }


#ifdef CBF_HAVE_XSD

#endif

} // namespace

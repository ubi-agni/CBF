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

  inline void limit(const double current_pos, double& next_pos, double& next_vel,
                    const double lower_pos, const double upper_pos, const double margin,
                    const double lower_vel, const double upper_vel, const double timestep) {
    // move back into bounds if neccessary
    if (current_pos > upper_pos) {
      next_vel = std::max(lower_vel, (upper_pos-current_pos)/timestep);
      goto UPDATE_POS;
    } else if (current_pos < lower_pos) {
      next_vel = std::min(upper_vel, (lower_pos-current_pos)/timestep);
      goto UPDATE_POS;
    }

    /*** current_pos is within bounds ***/
    // limit velocity
    if (next_vel > upper_vel) {
      next_vel = upper_vel;
      next_pos = current_pos + next_vel*timestep;
    } else if (next_vel < lower_vel) {
      next_vel = lower_vel;
      next_pos = current_pos + next_vel*timestep;
    }

    // upper pos limit
    if (next_pos > upper_pos) {
      next_vel = (upper_pos-current_pos)/timestep;
      goto UPDATE_POS;
    } else if (next_pos > (upper_pos-margin) && next_vel > 0) {
      next_vel *= (upper_pos-next_pos)/margin;
      goto UPDATE_POS;
    }

    // lower pos limit
    if (next_pos < lower_pos) {
      next_vel = (lower_pos-current_pos)/timestep;
      goto UPDATE_POS;
    } else if (next_pos < (lower_pos+margin) && next_vel < 0) {
      next_vel *= (next_pos-lower_pos)/margin;
      goto UPDATE_POS;
    }

    return;

UPDATE_POS:  // finally update next_pos
    next_pos = current_pos + next_vel*timestep;
  }

  void DampedResourceLimiter::limit(const FloatVector &current_pos, const FloatVector &current_vel,
                                    FloatVector &next_pos, FloatVector &next_vel) {
    for (unsigned int i=0; i<current_pos.size(); i++) {
      CBF::limit(current_pos(i), next_pos(i), next_vel(i), 
                 m_PosLowerLimit(i), m_PosUpperLimit(i), m_PosMargin(i),
                 -m_VelLimit(i), m_VelLimit(i), m_TimeStep);
    }
  }

  void DampedResourceLimiter::setMargin(const FloatVector& pos_margin)
  {
    assert(m_PosMargin.size() == pos_margin.size());
    m_PosMargin = pos_margin;
  }

  void UniformDampedResourceLimiter::limit(const FloatVector &current_pos, const FloatVector &current_vel,
                                           FloatVector &next_pos, FloatVector &next_vel) {
    double scale = 1.0;
    for (unsigned int i=0; i<current_pos.size(); i++) {
      double old = next_vel(i);
      CBF::limit(current_pos(i), next_pos(i), next_vel(i), 
                 m_PosLowerLimit(i), m_PosUpperLimit(i), m_PosMargin(i),
                 -m_VelLimit(i), m_VelLimit(i), m_TimeStep);
      next_pos(i) = old;  // store old velocity
      
      if (scale < 0.0);
      else if (old != next_vel(i) && ((old >= 0 && next_vel(i) <= 0) || (old <= 0 && next_vel(i) >= 0)))
        scale = -1.0;  // moving back into position bounds: cannot apply uniform scaling
      else if (old > 1.e-6)  // robustness for small velocities
        scale = std::min(scale, next_vel(i) / old);
    }

    if (scale > 0.0 && scale < 1.0)
      next_vel = scale * next_pos;  // scale based on original velocity stored in next_pos above

    next_pos = current_pos + m_TimeStep * next_vel;
  }

#ifdef CBF_HAVE_XSD
#endif

} // namespace

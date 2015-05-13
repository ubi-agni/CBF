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
 * limiter.h
 *
 * Abstract limiter to limit an object value
 *
 *  Created on May 13, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef LIMITER_H
#define LIMITER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class Limiter; }

namespace CBF {

struct Limiter : public Object {

  Limiter(const CBFSchema::Limiter &xml_instance, ObjectNamespacePtr object_namespace);

  Limiter(const Float timestep, const unsigned int dim_pos, const unsigned int dim_vel);

  virtual ~Limiter() { }

  void set_limit_pos(
      const FloatVector pos_upper_limit,
      const FloatVector pos_lower_limit);

  void set_limit_vel(const FloatVector vel_limit);

  virtual void limit(
      const FloatVector &current_pos,
      const FloatVector &current_vel,
      FloatVector &next_pos,
      FloatVector &next_vel) = 0;

  protected:
    Float m_TimeStep;
    Float m_Dim;

    FloatVector m_PosUpperLimit;
    FloatVector m_PosLowerLimit;
    FloatVector m_VelLimit;

    virtual void resize_variables(unsigned int dim_pos, unsigned int dim_vel);

};

typedef boost::shared_ptr<Limiter> LimiterPtr;


} // namespace

#endif // LIMITER_H

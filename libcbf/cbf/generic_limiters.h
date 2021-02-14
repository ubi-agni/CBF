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
 * generic_limiters.h
 *
 * basic limiters
 *
 *  Created on May 13, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef GENERIC_LIMITERS_H
#define GENERIC_LIMITERS_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>
#include <cbf/limiter.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class Limiter; }

namespace CBF {

struct NullLimiter : public Limiter {

  NullLimiter(const Float timestep, const unsigned int dim) : Limiter(timestep, dim, dim) { }

  void limit(const FloatVector &current_pos, const FloatVector &current_vel,
             FloatVector &next_pos, FloatVector &next_vel) override {}
};

struct DampedResourceLimiter : public Limiter {

  DampedResourceLimiter(const Float timestep, const unsigned int dim);

  void limit(const FloatVector &current_pos, const FloatVector &current_vel,
             FloatVector &next_pos, FloatVector &next_vel) override;

  void setMargin(const FloatVector& pos_margin);

  protected:
    FloatVector m_PosMargin;
};

struct UniformDampedResourceLimiter : public DampedResourceLimiter {
  using DampedResourceLimiter::DampedResourceLimiter;
  void limit(const FloatVector &current_pos, const FloatVector &current_vel,
             FloatVector &next_pos, FloatVector &next_vel) override;
};

typedef boost::shared_ptr<NullLimiter> NullLimiterPtr;
typedef boost::shared_ptr<DampedResourceLimiter> DampedResourceLimiterPtr;
typedef boost::shared_ptr<UniformDampedResourceLimiter> UniformDampedResourceLimiterPtr;

} // namespace

#endif // GENERIC_LIMITERS_H

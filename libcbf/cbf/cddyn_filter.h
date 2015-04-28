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
 * cddyn_task_space_planner.h
 *
 * This planner generates velocity continous task space trajectory based on cd dynamics
 *
 *  Created on Apr. 20, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef CDDYN_FILTER_H
#define CDDYN_FILTER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/filter.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class CDDynFilter; }

namespace CBF {

  struct CDDynFilter : public Filter {
      CDDynFilter(const CBFSchema::CDDynFilter &xml_instance, ObjectNamespacePtr object_namespace);

      CDDynFilter(const Float default_timestep,
                  const unsigned int state_dim,
                  const unsigned int state_vel_dim,
                  const Float wn=1.0);

      void reset(const FloatVector &state, const FloatVector &state_vel);

      void update_filtered_velocity(const FloatVector &state_error,
                                    const FloatVector &target_state,
                                    const FloatVector &target_state_vel,
                                    const Float timestep);

      void set_wn(const Float frequency);

    private:
      Float m_WN;
  };

  typedef boost::shared_ptr<CDDynFilter> CDDynFilterPtr;

} // namespace


#endif // CDDYN_FILTER_H

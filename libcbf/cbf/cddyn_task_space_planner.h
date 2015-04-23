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

#ifndef CDDYN_TASK_SPACE_PLANNER_H
#define CDDYN_TASK_SPACE_PLANNER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/task_space_planner.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <cd_dynamics/CDDynamics.hpp>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class CDDynTaskSpacePlanner; }

namespace CBF {

  struct CDDynTaskSpacePlanner : public TaskSpacePlanner {
      CDDynTaskSpacePlanner(const CBFSchema::CDDynTaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace);

      CDDynTaskSpacePlanner(Float timestep, PotentialPtr potential, const Float wn=1.0) :
        TaskSpacePlanner(timestep, potential)
      {
        m_Planner = new CDDynamics(potential->sensor_dim(), timestep, wn);

        m_UnitVelocity.resize(potential->task_dim());
        set_pontial(potential);

      }

      void reset(const FloatVector &pos, const FloatVector &step);

      void update(const std::vector<FloatVector > &ref);

      void get_task_step(FloatVector &result, const FloatVector &current_pos);

      void set_wn(const Float frequency);

    private:
      Float m_WN;
      CDDynamics *m_Planner;

      FloatVector m_UnitVelocity;
  };

  typedef boost::shared_ptr<CDDynTaskSpacePlanner> CDDynTaskSpacePlannerPtr;

} // namespace


#endif // CDDYN_TASK_SPACE_PLANNER_H

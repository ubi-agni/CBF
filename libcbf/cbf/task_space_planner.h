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
 * task_space_planner.h
 *
 * It is an abstract task space trajectory palnner
 *
 *  Created on April. 17, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef TASK_SPACE_PLANNER_H
#define TASK_SPACE_PLANNER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <cbf/potential.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class TaskSpacePlanner; }

namespace CBF {

struct TaskSpacePlanner : public Object {
  TaskSpacePlanner(const CBFSchema::TaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace);

  TaskSpacePlanner(Float timestep, PotentialPtr potential) :
    Object("TaskSpacePlanner")
  {
    m_TimeStep = timestep;
    m_Potential = potential;

    m_ClampTaskVel = false;

    resize_variables(m_Potential->sensor_dim(), m_Potential->task_dim());
  }

  virtual ~TaskSpacePlanner() { }

  virtual void reset(const FloatVector &pos, const FloatVector &step) = 0;

  virtual void update(const std::vector<FloatVector > &ref) = 0;

  virtual void get_task_step(FloatVector &result, const FloatVector &current_pos) = 0;

  unsigned int dim() { return m_Pos.size(); }

  FloatVector &get_pos() { return m_Pos; }

  FloatVector &get_current_task_step() { return m_TaskStep; }

  void set_pontial(PotentialPtr potential) { m_Potential = potential; }

  void set_time_step(Float timestep) { m_TimeStep = timestep; }

  Float get_time_step() { return m_TimeStep; }

  void set_max_task_vel(FloatVector &vel);

  void remove_max_task_vel();

  protected:
    Float m_TimeStep;

    FloatVector m_Pos;
    FloatVector m_TaskStep;
    FloatVector m_TaskAccel;

    bool m_ClampTaskVel;
    FloatVector m_MaxTaskVel;

    PotentialPtr m_Potential;

  private :
    void resize_variables(unsigned int dim, unsigned int dim_grad);


};

typedef boost::shared_ptr<TaskSpacePlanner> TaskSpacePlannerPtr;


} // namespace

#endif // TASK_SPACE_PLANNER_H

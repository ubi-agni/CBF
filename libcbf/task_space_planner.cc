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
 * task_space_planner.cc
 *
 * It is an abstract task space trajectory palnner
 *
 *  Created on April. 17, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/task_space_planner.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

  void TaskSpacePlanner::set_max_task_vel(FloatVector &vel)
  {
    m_ClampTaskVel = true;
    m_MaxTaskVel = vel;
  }

  void TaskSpacePlanner::remove_max_task_vel()
  {
    m_ClampTaskVel = false;
  }


  void TaskSpacePlanner::resize_variables(unsigned int dim, unsigned int dim_grad)
  {
    m_TaskPos    = FloatVector::Zero(dim);

    m_TaskVel    = FloatVector::Zero(dim_grad);
    m_TaskAccel  = FloatVector::Zero(dim_grad);

    m_MaxTaskVel = FloatVector::Zero(dim_grad);
  }

#ifdef CBF_HAVE_XSD
/*
  TaskSpacePlanner::TaskSpacePlanner(const CBFSchema::TaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace) :
    Object(xml_instance, object_namespace)
  {
    CBF_DEBUG("Constructor TaskSpacePlanner");
    m_MaxTaskVel = xml_instance.MaxTaskVel();
  }

  template <> XMLFactory<TaskSpacePlanner>
    *XMLFactory<TaskSpacePlanner>::m_Instance = 0;
*/
#endif

} // namespace

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
 * cddyn_task_space_planner.cc
 *
 * This planner generates velocity continous task space trajectory based on cd dynamics
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/cddyn_task_space_planner.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

void CDDynTaskSpacePlanner::reset(const FloatVector &pos, const FloatVector &step)
{
  m_Pos = pos;
  m_TaskStep = step;

  m_Planner->SetStateTarget(pos, pos);
  m_Planner->SetStateVel(m_TaskStep/m_TimeStep);
}

void CDDynTaskSpacePlanner::update(const std::vector<FloatVector> &ref)
{
  FloatVector m_Target(m_Pos.size());

  m_Potential->gradient(m_UnitVelocity, ref, m_Pos);

  m_Potential->integration(m_Target, m_Pos, m_UnitVelocity, 1.0);

  m_Planner->SetTarget(m_Target);
  m_Planner->Update();
  m_Planner->GetState(m_Pos);

}

void CDDynTaskSpacePlanner::get_task_step(FloatVector &result, const FloatVector &current_pos)
{
  std::vector<FloatVector> ref = std::vector<FloatVector>(1, FloatVector(current_pos.size()));
  ref[0] = m_Pos;

  m_Potential->gradient(result, ref, current_pos);
}

void CDDynTaskSpacePlanner::set_wn(const Float frequency)
{
  m_Planner->SetWn(frequency);
}

  #ifdef CBF_HAVE_XSD
/*
  CDDynTaskSpacePlanner::CDDynTaskSpacePlanner(const CBFSchema::CDDynTaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace) :
    TaskSpacePlanner(xml_instance, object_namespace)
  {
    CBF_DEBUG("[CDDynTaskSpacePlanner(const CDDynTaskSpacePlannerType &xml_instance)]: yay!");
    CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());
    //m_Coefficient = xml_instance.Coefficient();
  }

  static XMLDerivedFactory<
    CDDynTaskSpacePlanner,
    CBFSchema::CDDynTaskSpacePlanner
  > x1;
*/
  #endif

} // namespace

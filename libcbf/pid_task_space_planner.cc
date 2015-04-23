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

#include <cbf/pid_task_space_planner.h>
#include <cbf/xml_object_factory.h>

namespace CBF {


void PIDTaskSpacePlanner::reset(const FloatVector &pos, const FloatVector &step)
{
  m_Pos = pos;
  m_TaskStep = step;

  set_gain(1.0, 0.0, 0.0);
}

void PIDTaskSpacePlanner::update(const std::vector<FloatVector> &ref)
{
  FloatVector lNextPos(dim());

  m_Potential->gradient(m_ErrorInTaskSpace, ref, m_Pos);

  // P control
  m_TaskStep = m_ErrorInTaskSpace*m_GainP;

  // Integration
  m_Potential->integration(lNextPos, m_Pos, m_TaskStep, m_TimeStep);

  m_Pos = lNextPos;

}

void PIDTaskSpacePlanner::get_task_step(FloatVector &result, const FloatVector &current_pos)
{
  std::vector<FloatVector > ref;
  ref.push_back(m_Pos);

  m_Potential->gradient(result, ref, current_pos);
}

void PIDTaskSpacePlanner::set_gain(const Float gain_p, const Float gain_i, const Float gain_d)
{
  m_GainP = gain_p;
  m_GainI = gain_i;
  m_GainD = gain_d;
}


  #ifdef CBF_HAVE_XSD
/*
  PIDTaskSpacePlanner::PIDTaskSpacePlanner(const CBFSchema::PIDTaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace) :
    TaskSpacePlanner(xml_instance, object_namespace)
  {
    CBF_DEBUG("[PIDTaskSpacePlanner(const PIDTaskSpacePlannerType &xml_instance)]: yay!");
    CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());
  }

  static XMLDerivedFactory<
    PIDTaskSpacePlanner,
    CBFSchema::PIDTaskSpacePlanner
  > x1;
*/
  #endif

} // namespace

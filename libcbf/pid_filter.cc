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

#include <cbf/pid_filter.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

void PIDFilter::reset(const FloatVector &state, const FloatVector &state_vel)
{
  m_TargetState = state;
  m_FilteredState = state;

  m_TargetStateVel = state_vel;
  m_FilteredStateVel = state_vel;
}

void PIDFilter::update_filtered_velocity(const FloatVector &state_error,
                                         const FloatVector &target_state,
                                         const FloatVector &target_state_vel,
                                         const Float timestep) {

  m_TargetState    = target_state;
  m_TargetStateVel = target_state_vel;

  m_StateAccel = state_error*m_GainP +(target_state_vel-m_FilteredStateVel)*m_GainD;

  m_FilteredStateVel += m_StateAccel*timestep;
}

void PIDFilter::set_gain(const Float gain_p, const Float gain_i, const Float gain_d)
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

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
 * cddyn_filter.cc
 *
 * This filter based on cd_dynamics
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/cddyn_filter.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

CDDynFilter::CDDynFilter(const Float default_timestep,
                         const unsigned int state_dim,
                         const unsigned int state_vel_dim,
                         const Float wn) :
  Filter(default_timestep, state_dim, state_vel_dim)
{
  m_WN = wn;
}

void CDDynFilter::reset(const FloatVector &state, const FloatVector &state_vel)
{
  m_TargetState = state;
  m_TargetStateVel = state_vel;

  m_FilteredState = state;
  m_FilteredStateVel = state_vel;
}

void CDDynFilter::update(
    const FloatVector &state,
    const FloatVector &state_vel,
    const Float timestep)
{
  m_TargetState = state;
  m_TargetStateVel = state_vel;

  // critical damped dynamics computation
  diff(m_StateDiff, m_TargetState, m_FilteredState);

  // compute accel x_ddot(t)
  m_StateAccel = m_StateDiff*(m_WN*m_WN) + (m_TargetStateVel-m_FilteredStateVel)*(2.0*m_WN);

  // update
  //x(t+1) = x(t)+x_dot(t)*dt + 0.5*x_ddot(t)*dt*dt;
  //x_dot(t+1) = x_dot(t) + x_ddot(t)*dt;

  integration(m_FilteredState, m_FilteredState, m_FilteredStateVel, timestep);
  m_FilteredState += 0.5*m_StateAccel*timestep*timestep;

  m_FilteredStateVel += m_StateAccel*timestep;
}

void CDDynFilter::set_wn(const Float frequency)
{
  m_WN = frequency;
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

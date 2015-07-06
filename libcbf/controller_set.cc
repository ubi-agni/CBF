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
 * controller_set.cc
 *
 * Controller_set is an abstract controller to be implemented by users according to their needs
 * This controller has a single primitive controller.
 * Users can create the controller by combining different CBF modules.
 *
 *  Created on May 20, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/controller_set.h>

namespace CBF {

  ControllerSet::ControllerSet(ResourcePtr resource)
    : Object("ControllerSet")
  {
    m_Resource = resource;
    m_Dim = m_Resource->dim();
    m_State = STATE_CONTROLLERSET_IDLE;
  }

  void ControllerSet::reset()
  {
    reset(m_Resource->get_position(), m_Resource->get_velocity());
  }

  void ControllerSet::reset(const Eigen::VectorXd joint, const Eigen::VectorXd joint_vel)
  {
    m_Controller->reset(joint, joint_vel);
    m_State = STATE_CONTROLLERSET_INITIALIZED;
  }

} // namespace


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
 * controller_set.h
 *
 * Controller_set has a primitive controller
 * Users can create the primitive controller by combining different CBF modules
 *
 *  Created on May 20, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef CONTROLLER_SET_H
#define CONTROLLER_SET_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/primitive_controller.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>
#include <cbf/composite_resource.h>

#include <boost/shared_ptr.hpp>

enum ControllersetState {
  STATE_CONTROLLERSET_IDLE,
  STATE_CONTROLLERSET_INITIALIZED,
  STATE_CONTROLLERSET_WORKING,
  STATE_CONTROLLERSET_FINISHED};

namespace CBFSchema { class ControllerSet; }

namespace CBF {

  struct ControllerSet : public Object {
      ControllerSet(const CBFSchema::ControllerSet &xml_instance, ObjectNamespacePtr object_namespace);

      ControllerSet(ResourcePtr resource);

      virtual void update() = 0;

      virtual void stop() = 0;

      void reset();

      void reset(const Eigen::VectorXd joint, const Eigen::VectorXd joint_vel);

      unsigned int dim() { return m_Dim; }

      PrimitiveControllerPtr get_primitive_controller() { return m_Controller; }

      ControllersetState get_state() { return m_State; }

      ResourcePtr get_resource() { return m_Resource; }

    protected :
      unsigned int m_Dim;

      ControllersetState m_State;

      PrimitiveControllerPtr m_Controller;

      ResourcePtr m_Resource;
  };

  typedef boost::shared_ptr<ControllerSet > ControllerSetPtr;

} // namespace


#endif // CONTROLLER_SET_H

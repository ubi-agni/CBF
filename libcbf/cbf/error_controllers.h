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
 * error_controllers.h
 *
 * Implementations of some controllers that minimize the position and velocity errors
 *
 *  Created on May 05, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef ERROR_CONTROLLERS_H
#define ERROR_CONTROLLERS_H

#include <cbf/error_control.h>

namespace CBFSchema {
  class PDPositionControl;
  class PDPositionVelocityControl; }

namespace CBF {

  /**
    PDPositionControl compute task-space velocity so as to reduce the error between the reference and actual sensor values
    It use only position error information (x_ref - x_actual) based on PD controller
  */
  struct PDPositionControl : public ErrorControl {
    PDPositionControl(const CBFSchema::PDPositionControl &xml_instance, ObjectNamespacePtr object_namespace);

    PDPositionControl(const Float default_timestep,
                      const unsigned int dim,
                      const Float gain_p = 1.0,
                      const Float gain_d = 0.0);

    void set_gain(const Float gain_p, const Float gain_d);

    virtual void reset();

    virtual void update(
        const FloatVector &position_error,
        const FloatVector &velocity_error,
        const Float timestep);

    protected :
      Float m_GainP;
      Float m_GainD;
  };

  typedef boost::shared_ptr<PDPositionControl> PDPositionControlPtr;


} // namespace

#endif // ERROR_CONTROLLERS_H

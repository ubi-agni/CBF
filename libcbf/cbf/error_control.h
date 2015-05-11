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
 * error_control.h
 *
 * Abstract controller to minimize the position and velocity errors
 *
 *  Created on May 05, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef ERROR_CONTROL_H
#define ERROR_CONTROL_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class ErrorControl; }

namespace CBF {

struct ErrorControl : public Object {

  ErrorControl(const CBFSchema::ErrorControl &xml_instance, ObjectNamespacePtr object_namespace);

  ErrorControl(const Float default_timestep, const unsigned int dim);

  virtual ~ErrorControl() { }

  virtual void reset() = 0;

  virtual void update(
      const FloatVector &position_error,
      const FloatVector &velocity_error,
      const Float timestep) = 0;

  virtual FloatVector &get() { return m_Result; }

  unsigned int dim() { return m_Error.size(); }

  void set_time_step(const Float timestep) { m_TimeStep = timestep; }

  Float get_time_step() { return m_TimeStep; }

  protected:
    Float m_TimeStep;

    FloatVector m_Error;

    FloatVector m_ErrorDerivative;

    FloatVector m_VelocityError;

    FloatVector m_VelocityErrorDerivative;

    FloatVector m_Result;

  private :
    void resize_variables(unsigned int dim);

};

typedef boost::shared_ptr<ErrorControl> ErrorControlPtr;


} // namespace

#endif // ERROR_CONTROL_H

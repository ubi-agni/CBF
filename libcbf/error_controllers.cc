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
 * error_control.cc
 *
 * Abstract controller to minimize the position and velocity errors
 *
 *  Created on May 05, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/error_controllers.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

  PDPositionControl::PDPositionControl(
      const Float default_timestep,
      const unsigned int dim,
      const Float gain_p,
      const Float gain_d) :
    ErrorControl(default_timestep, dim)
  {
    set_gain(gain_p, gain_d);
  }

  void PDPositionControl::set_gain(const Float gain_p, const Float gain_d)
  {
    if(gain_p<=0) {
      CBF_THROW_RUNTIME_ERROR(m_Name << ": control gain_p (" << gain_p << ") must be bigger than 0");
    }

    m_GainP = gain_p;
    m_GainD = gain_d;
  }

  void PDPositionControl::reset()
  {
    m_Error.setZero();
    m_ErrorDerivative.setZero();

    m_VelocityError.setZero();
    m_VelocityErrorDerivative.setZero();
  }

  void PDPositionControl::update(
      const FloatVector &position_error,
      const FloatVector &velocity_error,
      const Float timestep)
  {
    m_ErrorDerivative = (position_error-m_Error)/timestep;
    m_Error = position_error;

    m_VelocityErrorDerivative = (velocity_error-m_VelocityError)/timestep;
    m_VelocityError = velocity_error;

    m_Result = m_Error*m_GainP/timestep + m_ErrorDerivative*m_GainD;
  }



#ifdef CBF_HAVE_XSD
/*
  PDPositionControl::PDPositionControl(const CBFSchema::PDPositionControl &xml_instance, ObjectNamespacePtr object_namespace) :
  {  }


*/
#endif

} // namespace

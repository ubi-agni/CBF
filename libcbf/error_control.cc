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

#include <cbf/error_control.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

  ErrorControl::ErrorControl(const Float default_timestep, const unsigned int dim) :
    Object("ErrorControl")
  {
    m_TimeStep = default_timestep;
    resize_variables(dim);
  }

  void ErrorControl::resize_variables(unsigned int dim)
  {
    m_Error = FloatVector::Zero(dim);
    m_ErrorDerivative = FloatVector::Zero(dim);
    m_VelocityError = FloatVector::Zero(dim);
    m_VelocityErrorDerivative = FloatVector::Zero(dim);
    m_Result = FloatVector::Zero(dim);
  }

#ifdef CBF_HAVE_XSD
  /*
  ErrorControl::ErrorControl(const CBFSchema::ErrorControl &xml_instance, ObjectNamespacePtr object_namespace) :
    Object(xml_instance, object_namespace)
  {
    CBF_DEBUG("Constructor ErrorControl");

  }

  template <> XMLFactory<ErrorControl>
    *XMLFactory<ErrorControl>::m_Instance = 0;
  */
#endif

} // namespace

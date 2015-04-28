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
 * filter.cc
 *
 * Abstract filter
 *
 *  Created on April. 27, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/filter.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

  void Filter::resize_variables(unsigned int state_dim, unsigned int state_vel_dim)
  {
    m_TargetState   = FloatVector::Zero(state_dim);
    m_FilteredState = FloatVector::Zero(state_dim);

    m_TargetStateVel   = FloatVector::Zero(state_vel_dim);
    m_FilteredStateVel = FloatVector::Zero(state_vel_dim);
    m_StateAccel       = FloatVector::Zero(state_vel_dim);
  }

#ifdef CBF_HAVE_XSD
/*
  Filter::Filter(const CBFSchema::Filter &xml_instance, ObjectNamespacePtr object_namespace) :
    Object(xml_instance, object_namespace)
  {
    CBF_DEBUG("Constructor Filter");

  }

  template <> XMLFactory<Filter>
    *XMLFactory<Filter>::m_Instance = 0;
*/
#endif

} // namespace

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
 * limiter.cc
 *
 * Abstract limiter to limit an object value
 *
 *  Created on May 13, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/limiter.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {
  Limiter::Limiter(const Float timestep, const unsigned int dim_pos, const unsigned int dim_vel) :
    Object("Limiter")
  {
    m_TimeStep = timestep;

    resize_variables(dim_pos, dim_vel);
  }

  void Limiter::set_limit_pos(
      const FloatVector& pos_upper_limit,
      const FloatVector& pos_lower_limit) {
    assert(m_PosUpperLimit.size() == pos_upper_limit.size());
    assert(m_PosLowerLimit.size() == pos_lower_limit.size());
    m_PosUpperLimit = pos_upper_limit;
    m_PosLowerLimit = pos_lower_limit;
  }

  void Limiter::set_limit_vel(const FloatVector& vel_limit) {
    assert(m_VelLimit.size() == vel_limit.size());
    m_VelLimit = vel_limit;
  }


  void Limiter::resize_variables(unsigned int dim_pos, unsigned int dim_vel)
  {
    m_PosUpperLimit = FloatVector::Zero(dim_pos);
    m_PosLowerLimit = FloatVector::Zero(dim_pos);

    m_VelLimit = FloatVector::Zero(dim_vel);
  }

#ifdef CBF_HAVE_XSD
  //Limiter(const CBFSchema::Limiter &xml_instance, ObjectNamespacePtr object_namespace);
#endif

} // namespace

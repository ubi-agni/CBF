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
 * filter.h
 *
 * Abstract filter
 *
 *  Created on April. 27, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef FILTER_H
#define FILTER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace CBFSchema { class Filter; }

namespace CBF {

typedef void (type_function_diff) (
        FloatVector&,
        const FloatVector&,
        const FloatVector&);

typedef void (type_function_integration) (
        FloatVector&,
        const FloatVector&,
        const FloatVector&,
        const Float);

struct Filter : public Object {

  boost::function<type_function_diff > diff;

  boost::function<type_function_integration > integration;

  Filter(const CBFSchema::Filter &xml_instance, ObjectNamespacePtr object_namespace);

  Filter(const Float default_timestep, const unsigned int state_dim, const unsigned int state_vel_dim);

  virtual ~Filter() { }

  virtual void reset(const FloatVector &state, const FloatVector &state_vel) = 0;

  virtual void update(
      const FloatVector &state,
      const FloatVector &state_vel,
      const Float timestep) = 0;

  FloatVector &get_filtered_state() { return m_FilteredState; }

  FloatVector &get_filtered_state_vel() { return m_FilteredStateVel; }

  unsigned int dim() { return m_FilteredState.size(); }

  void set_time_step(const Float timestep) { m_TimeStep = timestep; }

  Float get_time_step() { return m_TimeStep; }

  void set_function_diff(const type_function_diff *f);

  void set_function_integration(const type_function_integration *f);

  protected:
    Float m_TimeStep;

    FloatVector m_TargetState;
    FloatVector m_TargetStateVel;

    FloatVector m_FilteredState;
    FloatVector m_FilteredStateVel;

    FloatVector m_StateDiff;

    FloatVector m_StateAccel;

    static void euler_integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep) { nextpos = currentpos+(taskvel*timestep); }

    static void euler_diff(
        FloatVector &result,
        const FloatVector &front,
        const FloatVector &tail) { result = front-tail; }

  private :
    void resize_variables(unsigned int state_dim, unsigned int state_vel_dim);

};

typedef boost::shared_ptr<Filter> FilterPtr;


} // namespace

#endif // FILTER_H

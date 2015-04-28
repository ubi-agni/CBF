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
 * dmp_filter.h
 *
 * This planner generates task space trajectory based on DMP [1][2]
 *  [1] A. Ijspeert et al. "Dynamical movment primitives... ", Neural Computations, 2013
 *  [2] A. Ude et al. "Orientation in Cartesian space ...", ICRA, 2014
 *
 *  Created on Apr. 23, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef DMP_FILTER_H
#define DMP_FILTER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/filter.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class DMPFilter; }

namespace CBF {

  struct DMPFilter : public Filter {
      DMPFilter(const CBFSchema::DMPFilter &xml_instance, ObjectNamespacePtr object_namespace);

      DMPFilter(const Float default_timestep,
                const unsigned int state_dim,
                const unsigned int state_vel_dim,
                const std::vector<FloatVector > &basis_weights,
                const Float tau=1.0,
                const Float alpha_movement=1.0,
                const Float beta_movement=1.0,
                const Float alpha_phase=1.0);

      void reset(const FloatVector &state, const FloatVector &state_vel);

      void update_filtered_velocity(const FloatVector &state_error,
                                    const FloatVector &target_state,
                                    const FloatVector &target_state_vel,
                                    const Float timestep);

      void set_basis_function(const std::vector<FloatVector > &weights);

    private:
      FloatVector m_IntialAmplitude;
      FloatVector m_ScaledMovementVelocity;
      FloatVector m_Fx;

      unsigned int m_N;

      std::vector<FloatVector > m_BasisWeights;

      Float m_Tau;
      Float m_Alpha_Movement, m_Beta_Movement;
      Float m_Alpha_Phase;

      FloatVector m_C;
      FloatVector m_H;
      FloatVector m_Psi;

      Float m_Phase;

      void init_phase_parameters();

      Float compute_psi(const Float phase);
      void forcingterm(FloatVector fx, const Float phase);

  };

  typedef boost::shared_ptr<DMPFilter> DMPFilterPtr;

} // namespace


#endif // DMP_FILTER_H

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
 * dmp_task_space_planner.cc
 *
 * This planner generates task space trajectory based on DMP [1][2]
 *  [1] A. Ijspeert et al. "Dynamical movment primitives... ", Neural Computations, 2013
 *  [2] A. Ude et al. "Orientation in Cartesian space ...", ICRA, 2014
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include <cbf/dmp_filter.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

DMPFilter::DMPFilter(const Float default_timestep,
                     const unsigned int state_dim,
                     const unsigned int state_vel_dim,
                     const std::vector<FloatVector> &basis_weights,
                     const Float tau,
                     const Float alpha_movement,
                     const Float beta_movement,
                     const Float alpha_phase) :
  Filter(default_timestep, state_dim, state_vel_dim)
{
  m_N = basis_weights.size();
  if (m_N <= 1) {
    CBF_THROW_RUNTIME_ERROR(m_Name + ": DMP basis function is empty");
  }
  if (alpha_phase <= 0.0) {
    CBF_THROW_RUNTIME_ERROR(m_Name + ": DMP pahse decay parameter is wrong");
  }
  if (basis_weights[0].size() != state_vel_dim) {
    CBF_THROW_RUNTIME_ERROR(m_Name + ": DMP basis function dimension and potential dimension mismatch: " << basis_weights[0].size() << " is not equal to " << state_vel_dim);
  }

  m_IntialAmplitude.setZero(state_vel_dim);
  m_ScaledMovementVelocity.setZero(state_vel_dim);
  m_Fx.setZero(state_vel_dim);

  // set the dmp basis weights
  for (unsigned int i = 0; i < m_N; ++i) {
    m_BasisWeights.push_back(basis_weights[i]);
  }

  // set the dmp parameters
  m_Tau = tau;
  m_Alpha_Movement = alpha_movement;
  m_Beta_Movement = beta_movement;

  m_Alpha_Phase = alpha_phase;

  m_Phase = 1.0;
  init_phase_parameters();
}


void DMPFilter::reset(const FloatVector &state, const FloatVector &state_vel)
{
  m_TargetState = state;
  m_TargetStateVel = state_vel;

  m_FilteredState = state;
  m_FilteredStateVel = state_vel;

  m_ScaledMovementVelocity = m_FilteredStateVel*m_Tau;

  m_Phase = 1.0;
}

void DMPFilter::update_filtered_velocity(const FloatVector &state_error,
                                         const FloatVector &target_state,
                                         const FloatVector &target_state_vel,
                                         const Float timestep) {

  m_TargetState = target_state;
  m_TargetStateVel = target_state_vel;

  // compute initial amplitude
  if( m_Phase == 1.0)
  {
    m_IntialAmplitude = state_error;
  }

  // tau*x_dot = -alpah_x *x
  m_Phase += (-m_Alpha_Phase/m_Tau*m_Phase)*timestep;
  if (m_Phase<0.0) m_Phase = 0.0;

  // f(x)
  forcingterm(m_Fx, m_Phase);

  m_StateAccel = (m_Alpha_Movement*(m_Beta_Movement*state_error -m_ScaledMovementVelocity)+m_Fx )/(m_Tau*m_Tau);
  m_ScaledMovementVelocity += m_StateAccel*m_Tau*timestep;

  m_FilteredStateVel = (m_ScaledMovementVelocity/m_Tau);
}


void DMPFilter::set_basis_function(const std::vector<FloatVector > &weights)
{
  weights.size();
}

void DMPFilter::init_phase_parameters()
{
  m_C.resize(m_N);
  m_H.resize(m_N);
  m_Psi.resize(m_N);

  for (unsigned int i = 0; i < m_N; ++i) {
    m_C(i) = exp(-m_Alpha_Phase*((Float)i/((Float)m_N-1.0)));
  }

  for (unsigned int i = 0; i < m_N-1; ++i) {
    m_H(i) = 1.0/pow(m_C(i+1)-m_C(i), 2.0);
  }
  m_H(m_N-1) = m_H(m_N-2);
}


Float DMPFilter::compute_psi(const Float phase)
{
  Float sum_psi=0.0;
  for (unsigned int i = 0; i < m_N; ++i) {
    m_Psi(i) = exp(-m_H(i)*pow(phase-m_C(i), 2.0));
    sum_psi += m_Psi(i);
  }
  return sum_psi;
}

void DMPFilter::forcingterm(FloatVector fx, const Float phase)
{
  Float lsum_psi;
  lsum_psi = compute_psi(phase);

  fx.setZero(m_FilteredStateVel.size());
  for(unsigned int i=0; i<m_N; i++) {
    fx += m_BasisWeights[i]*m_Psi(i);
  }
  fx = fx/lsum_psi*phase*m_IntialAmplitude;

}


  #ifdef CBF_HAVE_XSD
/*
  DMPTaskSpacePlanner::DMPTaskSpacePlanner(const CBFSchema::DMPTaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace) :
    TaskSpacePlanner(xml_instance, object_namespace)
  {


  }

  static XMLDerivedFactory<
    DMPTaskSpacePlanner,
    CBFSchema::DMPTaskSpacePlanner
  > x1;
*/
  #endif

} // namespace

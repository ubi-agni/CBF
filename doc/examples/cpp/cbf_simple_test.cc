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

#include <cbf/primitive_controller.h>
#include <cbf/square_potential.h>
#include <cbf/cddyn_filter.h>
#include <cbf/bypass_filter.h>
#include <cbf/error_controllers.h>
#include <cbf/identity_transform.h>
#include <cbf/generic_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>
#include <cbf/convergence_criterion.h>
#include <cbf/generic_limiters.h>
#include <cbf/xml_factory.h>

#include <vector>

#define N_REF 3
#define N_DT (1./100.)

using namespace CBF;

int main() {
  unsigned int nJoints = 3;

  DummyReferencePtr reference(new DummyReference(1, N_REF));
  PotentialPtr potential(new SquarePotential(nJoints, nJoints));

	//! Create a PrimitiveController...
 	PrimitiveControllerPtr c(
		new PrimitiveController(
      N_DT,
			std::vector<ConvergenceCriterionPtr>(), 
			reference,
      CDDynFilterPtr(new CDDynFilter(N_DT, potential->sensor_dim(), potential->task_dim(), 1.0)),
      potential,
      PDPositionControlPtr(new PDPositionControl(N_DT, potential->task_dim())),
      SensorTransformPtr(new IdentitySensorTransform(potential->sensor_dim())),
      EffectorTransformPtr(new GenericEffectorTransform(potential->task_dim(), nJoints)),
			std::vector<SubordinateControllerPtr>(),
			CombinationStrategyPtr(new AddingStrategy),
      ResourcePtr(new DummyResource(nJoints)),
      BypassFilterPtr(new BypassFilter(N_DT, nJoints, nJoints)),
      NullLimiterPtr(new NullLimiter(N_DT, nJoints))
		)
	);

	FloatVector vec(3);
	vec[0] = vec[1] = vec[2] = 1;
	reference->set_reference(vec);

	/** 
		Run controller until convergence. Note that step() must be called
		before finished() to initialize internal state...
	*/
	do { c->step(); std::cout << "step" << std::endl; } while (c->finished() == false);
}

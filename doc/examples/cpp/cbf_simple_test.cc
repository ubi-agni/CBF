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
#include <cbf/potential.h>
#include <cbf/identity_transform.h>
#include <cbf/generic_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>

using namespace CBF;

int main() {
	//! Create a PrimitiveController...
	PrimitiveControllerPtr c
		(new PrimitiveController);

	//! A square potential function of dimension 3 and with coefficient 0.1...
	c->set_potential
		(PotentialPtr(new SquarePotential(3, 0.1)));

	//! A SensorTransform that does nothing..
	c->set_sensor_transform(boost::shared_ptr<SensorTransform>
		(new IdentitySensorTransform(3)));

	//! This effector transform will thus do nothing, too...
	c->set_effector_transform
		(EffectorTransformPtr
			(new GenericEffectorTransform(c->sensor_transform())));

	//! Create a DummyResource and bind to it...
	c->set_resource(ResourcePtr
		(new DummyResource(3)));

	CBF::DummyReferencePtr dr(new CBF::DummyReference(1,3));

	c->set_reference(dr);

	FloatVector vec(3);
	vec[0] = vec[1] = vec[2] = 1;

	dr->set_reference(vec);

	/** 
		Run controller until convergence. Note that step() must be called
		before finished() to initialize internal state...
	*/
	do { c->step(); std::cout << "step" << std::endl; } while (c->finished() == false);
}

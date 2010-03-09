/* -*- mode: c-non-suck; -*- */

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

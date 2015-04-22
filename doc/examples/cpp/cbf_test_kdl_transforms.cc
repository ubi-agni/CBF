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

#include <cbf/config.h>

#ifdef CBF_HAVE_QKDLVIEW
#include <QApplication>
#include <qkdlchainview.h>
#endif
#ifdef CBF_HAVE_SPACEMOUSE
#include <spacenavi.h>
#include <fcntl.h>
#endif

#include <cbf/cbf.h>
#include <cbf/pid_task_space_planner.h>
#include <kdl/chain.hpp>
#include <boost/assign/list_of.hpp>
#include <iostream>
#include <cstdlib>

#define NUM_OF_STEPS 1000

boost::shared_ptr<KDL::Chain> createChain (int iNumJointTriples=5) {
	//! Build up a fairly simple KDL::Chain
	boost::shared_ptr<KDL::Chain> chain (new KDL::Chain);
	for (unsigned int i = 0; i < iNumJointTriples; ++i) {
		chain->addSegment(
			KDL::Segment(KDL::Joint(KDL::Joint::RotX), 
							 KDL::Frame(KDL::Vector(0,0,0.1))));

		chain->addSegment(
			KDL::Segment(KDL::Joint(KDL::Joint::RotY), 
							 KDL::Frame(KDL::Vector(0,0,0.1))));

		chain->addSegment(
			KDL::Segment(KDL::Joint(KDL::Joint::RotZ), 
							 KDL::Frame(KDL::Vector(0,0,0.1))));
	}
	return chain;
}

CBF::PrimitiveControllerPtr createController (boost::shared_ptr<KDL::Chain> chain) {
	//! position + orientation control = 6D
	CBF::DummyReferencePtr reference(new CBF::DummyReference(1,6));
	reference->references()[0] = CBF::FloatVector::Unit(6,1);

	//! sensor transform for position + axis angle control
	std::vector<CBF::SensorTransformPtr> sensorTrafos = boost::assign::list_of
		(CBF::SensorTransformPtr (new CBF::KDLChainPositionSensorTransform(chain)))
		(CBF::SensorTransformPtr (new CBF::KDLChainAxisAngleSensorTransform(chain)));
	CBF::SensorTransformPtr sensorTrafo (new CBF::CompositeSensorTransform (sensorTrafos));

	//! potential composed from square (for position) + axis angle
	std::vector<CBF::PotentialPtr> potentials = boost::assign::list_of
    (CBF::PotentialPtr(new CBF::SquarePotential(3)))
    (CBF::PotentialPtr(new CBF::AxisAnglePotential()));
	CBF::PotentialPtr potential (new CBF::CompositePotential(potentials));

	unsigned int nJoints = chain->getNrOfJoints();
	CBF::PrimitiveControllerPtr controller (new CBF::PrimitiveController(
			1.0,
			std::vector<CBF::ConvergenceCriterionPtr>(),
			reference,
			potential,
      CBF::PIDTaskSpacePlannerPtr(new CBF::PIDTaskSpacePlanner(1./100., potential)),
			sensorTrafo,
			CBF::EffectorTransformPtr(new CBF::DampedGenericEffectorTransform(6, nJoints)),
			std::vector<CBF::SubordinateControllerPtr>(),
			CBF::CombinationStrategyPtr(new CBF::AddingStrategy),
			CBF::ResourcePtr(new CBF::DummyResource(nJoints))));
	return controller;
}

void* dev = NULL;
#ifdef CBF_HAVE_SPACEMOUSE
void openSpaceMouse () {
	dev = snavi_open(NULL, O_NONBLOCK);
	snavi_set_threshold(dev, 0);
}
void closeSpaceMouse () {
	snavi_close(dev);
}

CBF::FloatVector getSpaceMouseReference (const CBF::FloatVector &cur) {
	//! use spacemouse if available
	snavi_event_t ev;
	
	if (snavi_get_event(dev, &ev) >= 0 && 
		 ev.type == MotionEvent) {
		CBF::FloatVector axes(6);
		std::copy(ev.axes, ev.axes + 6, axes.data());
		std::cout << "axes: " << axes << std::endl;
	
		for (unsigned int i = 0; i < 6; ++i) {
			axes[i] = axes[i] * (1 - exp(0.00005 * -(axes[i] * axes[i])));
		}
	
		if (axes.squaredNorm() > 1000) return cur;

		const double factor = 0.0001;
		axes.head(3) *= 0.001;
		axes.head(3) += cur.head(3);

		CBF::FloatVector current_rot_ref(cur.tail(3));
		CBF::Quaternion  current_rot_ref_q;
		current_rot_ref_q.from_axis_angle3(current_rot_ref);
	
		std::cout << "current_q: " << current_rot_ref_q << std::endl;
	
		const float rot_factor = 0.05 * (1.0 / 500.0) / 2.0 * M_PI;
		CBF::Quaternion rot_x(cos(rot_factor * axes[3] / 2.0), sin(rot_factor * axes[3] / 2.0), 0, 0);
		CBF::Quaternion rot_y(cos(rot_factor * axes[4] / 2.0), 0, sin(rot_factor * axes[4] / 2.0), 0);
		CBF::Quaternion rot_z(cos(rot_factor * axes[5] / 2.0), 0, 0, sin(rot_factor * axes[5] / 2.0));

		rot_x.normalize(); rot_y.normalize(); rot_z.normalize();
	
		current_rot_ref_q = 
			rot_x * rot_y * rot_z
			* current_rot_ref_q 
			* rot_z.conjugate() * rot_y.conjugate() * rot_x.conjugate();

		std::cout << "next_q: " << current_rot_ref_q << std::endl;
	
		current_rot_ref_q.normalize();
		current_rot_ref_q.to_axis_angle3(current_rot_ref);
	
		axes.tail(3) = current_rot_ref;
		return axes;
	} else return cur;
}
#endif

CBF::FloatVector getReference (const CBF::FloatVector &cur) {
	CBF::FloatVector ref(cur.rows());
	ref.head(3) = CBF::FloatVector::Constant(3, 0.11);
	return ref;
}

int main(int argc, char *argv[]) {
	srand(1);

	boost::shared_ptr<KDL::Chain> chain = createChain ();
	CBF::PrimitiveControllerPtr controller = createController(chain);
	CBF::DummyReferencePtr ref 
		= boost::static_pointer_cast<CBF::DummyReference>(controller->reference());

	// randomly initialize the joints
	CBF::DummyResourcePtr resource 
		= boost::static_pointer_cast<CBF::DummyResource>(controller->resource());
	CBF::FloatVector res(resource->dim());
	for (unsigned int i = 0; i < resource->dim(); ++i)
		res[i] = ((float)rand()/(float)(RAND_MAX)) * 0.01;
	resource->set(res);

#ifdef CBF_HAVE_QKDLVIEW
	QApplication app(argc, argv);
	//! Initialize our cheap visualization :)
	KDLCV::QKDLChainView chain_view(chain.get());
	chain_view.show();
#endif

#ifdef CBF_HAVE_SPACEMOUSE
	openSpaceMouse ();
#endif

	unsigned int steps = 0;
	//! Execute until convergence or NUM_OF_STEPS, whatever comes first
	do {
		// retrieve spacemouse or static reference
		if (!dev)
			ref->set_reference(getReference(ref->get()[0]));
#ifdef CBF_HAVE_SPACEMOUSE
		else
			ref->set_reference(getSpaceMouseReference(ref->get()[0]));
#endif
		// controller step
		controller->step();

		// update chain view
#ifdef CBF_HAVE_QKDLVIEW
		for (unsigned int j = 0; j < chain_view.pose().size(); ++j)
			chain_view.pose()[j] = resource->get()[j];
	
		chain_view.show();
		chain_view.update();
		
		app.processEvents();
		usleep(10000);
#endif
	} while (steps++ < NUM_OF_STEPS && !controller->finished());
	std::cout << "[test_kdl_transforms]: steps: " << steps << std::endl;

#ifdef CBF_HAVE_SPACEMOUSE
	closeSpaceMouse ();
#endif
}

/**
	\example cbf_test_kdl_transforms.cc 

	@brief This example tests some KDL functionality (test_kdl_transforms.cc)
*/

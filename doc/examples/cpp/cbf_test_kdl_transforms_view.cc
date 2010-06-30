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

#include <QApplication>
#include <qkdlchainview.h>

#include <cbf/primitive_controller.h>
#include <cbf/dummy_reference.h>
#include <cbf/kdl_transforms.h>
#include <cbf/composite_potential.h>
#include <cbf/composite_transform.h>
#include <cbf/square_potential.h>
#include <cbf/effector_transform.h>
#include <cbf/sensor_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/combination_strategy.h>
#include <cbf/identity_transform.h>
#include <cbf/axis_angle_potential.h>
#include <cbf/generic_transform.h>
#include <cbf/utilities.h>
#include <cbf/quaternion.h>

#ifdef CBF_HAVE_SPACEMOUSE
	#include <spacenavi.h>
	#include <sys/stat.h>
	#include <fcntl.h>
#endif

#include <iostream>
#include <cstdlib>
#include <unistd.h>


#include <boost/shared_ptr.hpp>
#include <kdl/chain.hpp>

#define NUM_OF_STEPS 10000 

#define NUM_OF_JOINT_TRIPLES 5

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	//! Seed random number generator so we get reproducible results..
	srand(1);

	//! Use a primitive controller for this test
	CBF::PrimitiveControllerPtr primary_controller(
		new CBF::PrimitiveController(1, 1));

	CBF::DummyReferencePtr ref(new CBF::DummyReference(1, 6));
	primary_controller->set_reference(ref);
	

	//! Build up a fairly simple KDL::Chain (made up of 48 segments and joints)
	boost::shared_ptr<KDL::Chain> chain (new KDL::Chain);
	for (unsigned int i = 0; i < NUM_OF_JOINT_TRIPLES; ++i) {
		chain->addSegment(
			KDL::Segment(
				KDL::Joint::RotX, KDL::Frame(KDL::Vector(0,0,0.1))));

		chain->addSegment(
			KDL::Segment(
				KDL::Joint::RotY, KDL::Frame(KDL::Vector(0,0,0.1))));

		chain->addSegment(
			KDL::Segment(
				KDL::Joint::RotZ, KDL::Frame(KDL::Vector(0,0,0.1))));
	}

	//! Initialize our cheap visualization :)
	KDLCV::QKDLChainView chain_view(chain.get());
	chain_view.show();

	/** 
		Add sensor and effector transform that transform to/fro 
		position/orientation space. Here we assemble a 
		composite transform from one for cartesian EE position 
		and one for EE orientation in Axis Angle representation.
	*/
	std::vector<CBF::SensorTransformPtr> sensor_trafos;

	sensor_trafos.push_back
		(CBF::SensorTransformPtr
			(new CBF::KDLChainPositionSensorTransform(chain)));

	sensor_trafos.push_back
		(CBF::SensorTransformPtr
			(new CBF::KDLChainAxisAngleSensorTransform(chain)));

	primary_controller->set_sensor_transform
		(CBF::SensorTransformPtr
			(new CBF::CompositeSensorTransform(sensor_trafos)));

	//! We use the generic pseudo inverse based effector transform,.
	primary_controller->set_effector_transform
		(CBF::EffectorTransformPtr
			(new CBF::DampedGenericEffectorTransform(primary_controller->sensor_transform())));

	/**
		Add a potential function. The potential function is again a composite one,
		as orientation space needs to be treated differently depending on
		the representation
	*/
	std::vector<CBF::PotentialPtr> pots;
	pots.push_back(CBF::PotentialPtr(new CBF::SquarePotential(3,0.01)));
	pots.push_back(CBF::PotentialPtr(new CBF::AxisAnglePotential(0.1)));

	primary_controller->set_potential(CBF::PotentialPtr(new CBF::CompositePotential(pots)));
	
	//! Since no subordinate controller is used, this is really not necessary (TODO: Fix
	//! this, so it's really not necessary)
	primary_controller->set_combination_strategy(CBF::CombinationStrategyPtr(new CBF::AddingStrategy));

	//! Finally bind this controller to a dummy resource with 48 variables (one for each
	//! joint of the KDL::Chain
	CBF::DummyResourcePtr resource(new CBF::DummyResource(NUM_OF_JOINT_TRIPLES * 3));
	CBF::FloatVector res = resource->get();
	for (unsigned int i = 0; i < NUM_OF_JOINT_TRIPLES * 3; ++i)
		res[i] = ((float)rand()/(float)(RAND_MAX)) * 0.01;
	resource->set(res);

	primary_controller->set_resource(resource);


	//! Create a secondary controller that minimizes joint angles.
	CBF::PrimitiveControllerPtr secondary_controller(new CBF::AvoidSingularitiesController);

	secondary_controller->set_sensor_transform(
		CBF::SensorTransformPtr(
			new CBF::IdentitySensorTransform(
				primary_controller->effector_transform()->resource()->dim())));

	secondary_controller->set_effector_transform(
		CBF::EffectorTransformPtr(
			new CBF::DampedGenericEffectorTransform(
				secondary_controller->sensor_transform())));

	secondary_controller->set_potential(
		CBF::PotentialPtr(new CBF::SquarePotential(NUM_OF_JOINT_TRIPLES * 3, 0.01)));

	secondary_controller->set_resource(resource);

	/**
		The reference for the secondary controller is just the 0 vector as we want to
		minimize joint angles and their minimum is at 0 :)
	*/
	// secondary_controller->references().push_back(CBF::ublas::zero_vector<CBF::Float>(NUM_OF_JOINT_TRIPLES * 3));
	secondary_controller->set_reference(
		CBF::ReferencePtr(new CBF::DummyReference(1,NUM_OF_JOINT_TRIPLES * 3)));

	/**
		Finally add the secondary controller so its output is projected into the
		nullspace of the primary controller
	*/
	primary_controller->subordinate_controllers().push_back(secondary_controller);


#ifdef CBF_HAVE_SPACEMOUSE
	//! If there was libnavi present at configuration time, then the user
	//! can specify any argument. Then this example is controlled by the spacemouse
	std::cout << "HAVE_SPACEMOUSE" << std::endl;
	if (argc > 1)
	{
		snavi_event_t ev;
		void *dev = snavi_open(NULL, O_NONBLOCK);
		if (dev == 0)
			throw std::runtime_error("Could not open spacemouse device");
	
		snavi_set_threshold(dev, 0);
	
		unsigned int count = 0;
	
	
		while(true) {
			while(snavi_get_event(dev, &ev) >= 0) {
				//if (count > 100) exit(0);
	
				if (ev.type == MotionEvent) {
					std::cout << "." << std::flush;
					CBF::ublas::vector<CBF::Float> axes(6);
					std::copy(ev.axes, ev.axes + 6, axes.begin());
					std::cout << "axes: " << axes << std::endl;
	
					for (unsigned int i = 0; i < 6; ++i) {
						axes[i] = axes[i] * (1 - exp(0.00005 * -(axes[i] * axes[i])));
					}
	
					if (CBF::ublas::norm_2(axes) > 1000) continue;
	
					const double factor = 0.0001;
	
					ref->references()[0][0] += factor * axes[0];
					ref->references()[0][1] += -factor * axes[1];
					ref->references()[0][2] += -factor * axes[2];

					CBF::ublas::vector<CBF::Float> current_rot_ref(3);
					std::copy(
						ref->references()[0].begin() + 3,
						ref->references()[0].end(),
						current_rot_ref.begin());
	
					CBF::Quaternion current_rot_ref_q;
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

					//current_rot_ref_q = rot_y * current_rot_ref_q;
	
					std::cout << "next_q: " << current_rot_ref_q << std::endl;
	
					current_rot_ref_q.normalize();
					current_rot_ref_q.to_axis_angle3(current_rot_ref);
	
					std::cout << "next_q axis/angle " << current_rot_ref_q << std::endl;

					std::cout << "new_rot_ref: " << current_rot_ref << std::endl;
					std::copy(
						current_rot_ref.begin(),
						current_rot_ref.end(),
						ref->references()[0].begin() + 3
					);
	
					std::cout << "ref: " << primary_controller->reference()->get()[0] << std::endl;
				}
				primary_controller->step();
	
				if (count++ % 1 == 0) {
					//std::cout << "[test_kdl_transforms_view]: step: " << i << std::endl;
					for (unsigned int j = 0; j < chain_view.pose().size(); ++j)
					{
						chain_view.pose()[j] = primary_controller->effector_transform()->resource()->get()[j];
					}
	
					chain_view.show();
					chain_view.update();
	
					app.processEvents();
					usleep(10000);
				}
			}
		}
	return EXIT_SUCCESS;
	}
#endif

	unsigned int count = 0;

	float angle = 0;
	for (int runs = 0; runs < 60; ++runs)
	{

		//! Execute until convergence or NUM_OF_STEPS, whatever comes first
		unsigned int i = 0;
		do
		{
			std::cout << "[test_kdl_transforms]: setting reference" << std::endl;
			//primary_controller->references().clear();
			//primary_controller->references().push_back(CBF::ublas::zero_vector<CBF::Float>(6));
			ref->references()[0][0] = 0.11;//1.0 * sin(angle);
			ref->references()[0][1] = 0.11;//1.0 * cos(angle);
			ref->references()[0][2] = 0.11;//1.0 * cos(angle);


			ref->references()[0][3] = angle;//M_PI/2.0;
			ref->references()[0][4] = 0;//angle;
			ref->references()[0][5] = 0;//angle;

			primary_controller->step();

			if (count++ % 1 == 0) {
				std::cout << "[test_kdl_transforms_view]: step: " << i << std::endl;
				std::cout << "[test_kdl_transforms_view]: chain_view.pose().size(): " << chain_view.pose().size() << std::endl;
				for (unsigned int j = 0; j < chain_view.pose().size(); ++j)
				{
					chain_view.pose()[j] = primary_controller->effector_transform()->resource()->get()[j];
				}

				chain_view.show();
				chain_view.update();

				app.processEvents();
				usleep(10000);
			}
			angle = angle + M_PI / 542;
			angle = fmod(angle, 2.0 * M_PI);
		}
		while (i++ < NUM_OF_STEPS /*&& !primary_controller->finished()*/);
	}
	std::cout << std::endl;

}

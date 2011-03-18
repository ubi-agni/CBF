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
#include <cbf/composite_potential.h>
#include <cbf/kdl_transforms.h>
#include <cbf/effector_transform.h>
#include <cbf/sensor_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/combination_strategy.h>
#include <cbf/utilities.h>
#include <cbf/generic_transform.h>
#include <cbf/dummy_reference.h>

#include <iostream>
#include <cstdlib>

// #include <kdl/jacobian.hpp>
#include <kdl/chain.hpp>

#define NUM_OF_STEPS 1000
#define NUM_OF_RUNS 1


int main(int argc, char *argv[]) {
	srand(1);

	CBF::PrimitiveControllerPtr controller;
	{
		//! Build up a fairly simple KDL::Chain
		boost::shared_ptr<KDL::Chain> chain (new KDL::Chain);
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotX), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotY), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotZ), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotX), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotY), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotZ), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotX), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotY), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotZ), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotX), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotY), 
												 KDL::Frame(KDL::Vector(1,0,0))));
		chain->addSegment(KDL::Segment(KDL::Joint(KDL::Joint::RotZ), 
												 KDL::Frame(KDL::Vector(1,0,0))));
	
	
		CBF::DummyReferencePtr ref(new CBF::DummyReference(1,3));
		ref->references()[0] = CBF::ublas::unit_vector<CBF::Float>(3,1);

		//! Use a simple primitive controller for this test
		controller = CBF::PrimitiveControllerPtr(
			new CBF::PrimitiveController(
				1.0,
				std::vector<CBF::ConvergenceCriterionPtr>(),
				ref,
				CBF::PotentialPtr(new CBF::SquarePotential(3, 0.1)),
				CBF::SensorTransformPtr(new CBF::KDLChainPositionSensorTransform(chain)),
				CBF::EffectorTransformPtr(new CBF::PaddedEffectorTransform(3, 12, 1.5)),
				std::vector<CBF::SubordinateControllerPtr>(),
				CBF::CombinationStrategyPtr(new CBF::AddingStrategy),
				CBF::ResourcePtr(new CBF::DummyResource(12))
			)
		);

		unsigned int total_steps = 0;
		for (unsigned int run = 0; run < NUM_OF_RUNS; ++run) {
			std::cout << "[test_kdl_transforms]: run: " << run << std::endl;
			//! Execute until convergence or NUM_OF_STEPS, whatever comes first
			unsigned int i = 0;
			do {
				controller->step();

				//std::cout << "[test_kdl_transforms]: step: " << i << std::endl;
				++total_steps;
			}
			while (i++ < NUM_OF_STEPS && !controller->finished());
		}
		std::cout << "[test_kdl_transforms]: total_steps: " << total_steps << std::endl;
	}
}

/**
	\example cbf_test_kdl_transforms.cc 

	@brief This example tests some KDL functionality (test_kdl_transforms.cc)
*/

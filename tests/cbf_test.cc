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

/* -*- mode: c-non-suck; -*- */

#include <cbf/primitive_controller.h>

#include <cbf/potential.h>
#include <cbf/effector_transform.h>
#include <cbf/identity_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/sensor_transform.h>
#include <cbf/utilities.h>
#include <cbf/generic_transform.h>
#include <cbf/dummy_reference.h>
#include <cbf/quaternion.h>


#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <iostream>
#include <cstdlib>

#define NUM_OF_STEPS 10

namespace ublas = boost::numeric::ublas;

int main(int argc, char *argv[]) {
	//! Initialize pseudo random number gen with 0, so we get reproducable results..
	srand(0);

	{
		ublas::matrix<CBF::Float> m(3,3);
		for (unsigned int i = 0; i < 3; ++i)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				m(i,j) = (float)rand()/(float)RAND_MAX;
			}
		}
		std::cout << m << std::endl;

		//m = 20.0 * m;
		ublas::matrix<CBF::Float> res(3,3);
		CBF::pseudo_inverse(m, res);
		std::cout << "res: " << res << std::endl;

		std::cout << "should be identity: " << ublas::prod(m,res) << std::endl;
	}

	{
		std::cout << "Controller related stuff" << std::endl;

		//! Create a primitive controller.
		CBF::PrimitiveController controller;
	
		//! Create nessecary members of the primitive controller.
		controller.set_potential(CBF::PotentialPtr(new CBF::SquarePotential(6)));
	
		//! For this simple test we only use the identity transform
		controller.set_sensor_transform(CBF::SensorTransformPtr(new CBF::IdentitySensorTransform(6)));

		controller.set_effector_transform(CBF::EffectorTransformPtr(new CBF::GenericEffectorTransform(controller.sensor_transform())));
	
		//! And a dummy resource.
		CBF::ResourcePtr resource = CBF::ResourcePtr(new CBF::DummyResource(6));
	
		controller.set_resource(resource);
	
		//! Adding of a combination strategy is really unnecessary but the design dictates we 
		//! do it anyways (TODO: fix so this is not necessary anymore)
		controller.set_combination_strategy(CBF::CombinationStrategyPtr(new CBF::AddingStrategy));

		CBF::DummyReferencePtr ref(new CBF::DummyReference(1,6));
		controller.set_reference(ref);

		//! Set a reference to the controller.
		ref->references()[0][0] = 1;
		ref->references()[0][1] = 2;
		ref->references()[0][2] = 3;
		ref->references()[0][3] = 4;
		ref->references()[0][4] = 5;
		ref->references()[0][5] = 6;
	
		//! Run until convergance or NUM_OF_STEPS, whatever comes first
		unsigned int i = 0;
		do {
			controller.step(), std::cout << "[test_cbf]: step: " << i << std::endl;
		}
		while(i++ < NUM_OF_STEPS && !controller.finished());
	
		std::cout << "[testcbf]: Finished: " << controller.finished() << std::endl;
	}

	{
		//! Test quaterinon functions
		std::cout << "Quaternion stuff: " << std::endl;

		CBF::ublas::matrix<CBF::Float> m1 = CBF::ublas::identity_matrix<CBF::Float>(3,3);
		CBF::ublas::matrix<CBF::Float> m2 = CBF::ublas::zero_matrix<CBF::Float>(3,3);
		m2(0,1) = -1;
		m2(1,0) = 1;
		m2(2,2) = 1;

		CBF::Quaternion q = m2;
		std::cout << "m2: " << m2 << " " << "q: " << q << std::endl;

		CBF::Quaternion q1 = m1;
		CBF::Quaternion q2 = CBF::Quaternion(1.0, 0.3, 0.4, 0.5).normalize();

		std::cout << "q1: " << q1 << " q2: " << q2 << std::endl;
	
		for (int i = 0; i < 101; ++i) {
			CBF::ublas::matrix<CBF::Float> m3 = (CBF::ublas::matrix<CBF::Float>)CBF::qslerp(q1, q2, i/100.0);
			std::cout << "slerp(" << i/100.0 << "): " << CBF::qslerp(q1, q2, i/100.0) << std::endl;
			std::cout << "length: " << CBF::qslerp(q1, q2, i/100.0).length() << std::endl;
			//std::cout << "m: " << m << std::endl;
		}

		//! Create a rotation quaternion representing a rotation of PI/2 about the y-axis.
		CBF::Quaternion q4 = CBF::Quaternion(cos(M_PI/4.23), 0, sin(M_PI/4.23) * 1, 0).normalize();

		//! Convert it to a rotation matrix (3x3).
		CBF::ublas::matrix<CBF::Float> m4 = q4;
		std::cout << "q4: " << q4 << std::endl << "m4: " << m4 << std::endl;

		q4 = m4;
		m4 = q4;
		q4 = m4;
		m4 = q4;
		q4 = m4;
		m4 = q4;

		std::cout << "q4: " << q4 << std::endl << "m4: " << m4 << std::endl;

		//! Create a vector to be rotated.
		CBF::ublas::vector<CBF::Float> v(3);
		v[0] = 1;
		v[1] = 0;
		v[2] = 0;
		
		//! Output result of not rotating
		std::cout << "rotated: " << CBF::ublas::prod(CBF::ublas::identity_matrix<CBF::Float>(3,3),v) << std::endl;

		std::cout << "rotated: " << CBF::ublas::prod(m4, v) << std::endl;
	}

	{
		CBF::Quaternion q1;
		CBF::Quaternion q2;

		CBF::FloatVector v1(4);
		CBF::FloatVector v2(4);

		v1[0] = -M_PI/2.0; v1[1] = 1; v1[2] = 0; v1[3] = 0;
		v2[0] = M_PI/2.0; v2[1] = 1; v2[2] = 0; v2[3] = 0;

		q1 = v1;
		q2 = v2;

		q1.from_axis_angle(q1);
		q2.from_axis_angle(q2);

		CBF::Quaternion q3;
		q3 = q1.conjugate() * q2;

		q3.axis_angle();

		std::cout << "angle: " << q3.w/M_PI << std::endl;
	}

}



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


    Copyright 2009, 2010 Florian Paul Schmidt, Viktor Richter
*/

#include <cbf/primitive_controller.h>
#include <cbf/square_potential.h>
#include <cbf/identity_transform.h>
#include <cbf/generic_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>
#include <cbf/convergence_criterion.h>
#include <cbf/xml_factory.h>

#include <cbf/xcf_memory_sensor_transform.h>

#include <vector>

using namespace CBF;

int main(int argc, char *argv[]) {
	if(argc==3){
		DummyReferencePtr reference(new DummyReference(1,3));
		//create a SensorTransform and wrap it in XCFMemorySensorTransform
		SensorTransformPtr sTransPtr = SensorTransformPtr(new IdentitySensorTransform(3));
		SensorTransformPtr memorySensorTransform(new XCFMemorySensorTransform(argv[1], argv[2], sTransPtr));

		//! Create a PrimitiveController...
	 	PrimitiveControllerPtr c(
			new PrimitiveController(
				1.0,
				std::vector<ConvergenceCriterionPtr>(), 
				reference,
				PotentialPtr(new SquarePotential(3, 0.1)),
				memorySensorTransform,
//				SensorTransformPtr(new IdentitySensorTransform(3)),
				EffectorTransformPtr(new GenericEffectorTransform(3,3)),
				std::vector<SubordinateControllerPtr>(),
				CombinationStrategyPtr(new AddingStrategy),
				ResourcePtr(new DummyResource(3))
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
	} else {
		std::cout << "Please use >cbf_simple_test_memory 'memoryservername' 'resultName'" << std::endl;
	}
}

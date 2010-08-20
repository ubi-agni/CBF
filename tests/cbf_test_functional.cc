#include <cbf/functional.h>
#include <cbf/identity_transform.h>
#include <cbf/dummy_resource.h>

#include <iostream>
#include <boost/numeric/ublas/io.hpp>

using namespace CBF;


int main() {
	SensorTransformPtr id(new IdentitySensorTransform(9));	

	DummyResourcePtr r(new DummyResource(9));

	FloatVector vec(9);
	vec[0] = 1; vec[8] = -1;
	r->set(vec);

	SensorTransformPtr s(
		make_ApplyOperationSensorTransform(
			id,
			std::bind2nd(multiplies<FloatVector, double>(), 1.3),
			std::bind2nd(multiplies<FloatMatrix, double>(), 1.4)
		)
	);

	s->set_resource(r);
	s->update();

	std::cout << "s1" << std::endl;
	std::cout << "result   " << s->result() << std::endl;
	std::cout << "jacobian " << s->task_jacobian() << std::endl;

	SensorTransformPtr s2(
		make_ApplyOperationBlockWiseSensorTransform(
			id,
			std::bind2nd(multiplies<FloatVector, double>(), 1.3),
			std::bind2nd(multiplies<FloatMatrix, double>(), 1.4),
			3
		)
	);

	s2->set_resource(r);
	s2->update();

	std::cout << "s2" << std::endl;
	std::cout << "result   " << s2->result() << std::endl;
	std::cout << "jacobian " << s2->task_jacobian() << std::endl;

	SensorTransformPtr s3(
		new MultiplyOperationSensorTransform(
			id, 
			std::bind2nd(multiplies<FloatVector, double>(), 1.3),
			std::bind2nd(multiplies<FloatMatrix, double>(), 1.4)
		)
	);

	s3->set_resource(r);
	s3->update();

	std::cout << "s3" << std::endl;
	std::cout << "result   " << s3->result() << std::endl;
	std::cout << "jacobian " << s3->task_jacobian() << std::endl;


}

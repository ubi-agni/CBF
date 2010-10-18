#include <cbf/functional.h>
#include <cbf/identity_transform.h>
#include <cbf/dummy_resource.h>

#include <iostream>
#include <boost/numeric/ublas/io.hpp>

int main() {
	CBF::SensorTransformPtr id(new CBF::IdentitySensorTransform(9));	

	CBF::DummyResourcePtr r(new CBF::DummyResource(9));

	CBF::FloatVector vec(9);
	vec[0] = 1; vec[8] = -1;
	r->set(vec);

	CBF::SensorTransformPtr s(
		CBF::make_ApplyOperationSensorTransform(
			id,
			std::bind2nd(CBF::multiplies<CBF::FloatVector, double>(), 1.3),
			std::bind2nd(CBF::multiplies<CBF::FloatMatrix, double>(), 1.4)
		)
	);

	s->update(r->get());

	std::cout << "s1" << std::endl;
	std::cout << "result   " << s->result() << std::endl;
	std::cout << "jacobian " << s->task_jacobian() << std::endl;

	CBF::SensorTransformPtr s2(
		CBF::make_ApplyOperationBlockWiseSensorTransform(
			id,
			std::bind2nd(CBF::multiplies<CBF::FloatVector, double>(), 1.3),
			std::bind2nd(CBF::multiplies<CBF::FloatMatrix, double>(), 1.4),
			3
		)
	);

	s2->update(r->get());

	std::cout << "s2" << std::endl;
	std::cout << "result   " << s2->result() << std::endl;
	std::cout << "jacobian " << s2->task_jacobian() << std::endl;

	CBF::SensorTransformPtr s3(
		new CBF::MultiplyOperationSensorTransform(
			id, 
			std::bind2nd(CBF::multiplies<CBF::FloatVector, double>(), 1.3),
			std::bind2nd(CBF::multiplies<CBF::FloatMatrix, double>(), 1.4)
		)
	);

	s3->update(r->get());

	std::cout << "s3" << std::endl;
	std::cout << "result   " << s3->result() << std::endl;
	std::cout << "jacobian " << s3->task_jacobian() << std::endl;


}

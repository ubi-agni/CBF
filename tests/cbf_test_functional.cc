#include <cbf/functional.h>
#include <cbf/identity_transform.h>
#include <cbf/dummy_resource.h>

#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <functional>

int main() {
	CBF::SensorTransformPtr id(new CBF::IdentitySensorTransform(9));	

	CBF::DummyResourcePtr r(new CBF::DummyResource(9));

	CBF::FloatVector vec(9);
	vec[0] = 1; vec[3] = 1; vec[8] = -1;
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

	CBF::FloatVector v(3);
	v[0] = 1; v[1] = 2; v[2] = 3;

	CBF::FloatMatrix m(3,9);
	m(0,0) = 1;	m(0,1) = 2;	m(0,2) = 3; m(0,3) = 1;	m(0,4) = 2;	m(0,5) = 3; m(0,6) = 1;	m(0,7) = 2;	m(0,8) = 3;
	m(1,0) = 4;	m(1,1) = 5;	m(1,2) = 6; m(1,3) = 4;	m(1,4) = 5;	m(1,5) = 6; m(1,6) = 4;	m(1,7) = 5;	m(1,8) = 6;
	m(2,0) = 7;	m(2,1) = 8;	m(2,2) = 9; m(2,3) = 7;	m(2,4) = 8;	m(2,5) = 9; m(2,6) = 7;	m(2,7) = 8;	m(2,8) = 9;



	CBF::SensorTransformPtr s4(
		CBF::make_ApplyOperationBlockWiseSensorTransform(
			id,
			std::bind2nd(std::plus<CBF::FloatVector>(), v),
			std::bind2nd(std::plus<CBF::FloatMatrix>(), m),
			3
		)
	);

	s4->update(r->get());

	std::cout << "s4" << std::endl;
	std::cout << "result   " << s4->result() << std::endl;
	std::cout << "jacobian " << s4->task_jacobian() << std::endl;


	CBF::FloatVector init_vector = 
		boost::numeric::ublas::unit_vector<CBF::Float>(3, 0);

	CBF::SensorTransformPtr s5(
		CBF::make_BlockWiseAccumulateSensorTransform
		(
			id,
			std::plus<CBF::FloatVector>(),
			std::plus<CBF::FloatMatrix>(),
			boost::numeric::ublas::zero_vector<CBF::Float>(3),
			boost::numeric::ublas::zero_matrix<CBF::Float>(3,9),
			3
		)
	);
	s5->update(r->get());

	std::cout << "s5" << std::endl;
	std::cout << "result   " << s5->result() << std::endl;
	std::cout << "jacobian " << s5->task_jacobian() << std::endl;

}

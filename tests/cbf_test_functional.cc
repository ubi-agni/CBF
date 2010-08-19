#include <cbf/functional.h>
#include <cbf/identity_transform.h>
#include <cbf/dummy_resource.h>

#include <iostream>
#include <boost/numeric/ublas/io.hpp>

using namespace CBF;

template<typename T, typename U> 
struct multiplies { 
	typedef T first_argument_type;
	typedef U second_argument_type;
	typedef T result_type;
	T operator()(T const& t, U const& u) const { return t * u; } 
};

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

}

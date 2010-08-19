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
#if 0
	MapBlockWiseSensorTransformOperation<
		std::binder1st<std::multiplies<FloatVector> >,
		std::binder1st<std::multiplies<FloatMatrix> >
	> mul_test(
		std::bind1st(std::multiplies<FloatVector>(), 2.1),
		std::bind1st(std::multiplies<FloatMatrix>(), 3.3),
		3
	);

#if 0
	MapBlockWiseSensorTransformOperation<
		std::negate<FloatVector>, 
		std::negate<FloatMatrix>
	> neg(3);
#endif

	SensorTransformPtr id(new IdentitySensorTransform(9));	

#if 0
	OperationSensorTransform<
		MapBlockWiseSensorTransformOperation<
			std::negate<FloatVector>, 
			std::negate<FloatMatrix> 
		>
	> st(neg, id);
#endif 

	DummyResourcePtr r(new DummyResource(9));

	FloatVector vec(9);
	vec[0] = 1; vec[8] = -1;
	r->set(vec);

	st.set_resource(r);
	st.update();

	std::cout << "result   " << st.result() << std::endl;
	std::cout << "jacobian " << st.task_jacobian() << std::endl;
#endif

	SensorTransformPtr id(new IdentitySensorTransform(9));	

	//ublas::prod<FloatMatrix, Float> (ublas::identity_matrix<Float>(3,3), 3.0);

	ApplyOperationSensorTransform<
		std::binder2nd<multiplies<FloatVector, double> >,
		std::binder2nd<multiplies<FloatMatrix, double> >
	> st(
		id,
		std::bind2nd(multiplies<FloatVector, double>(), 1.3),
		std::bind2nd(multiplies<FloatMatrix, double>(), 1.4)
	);

	DummyResourcePtr r(new DummyResource(9));

	FloatVector vec(9);
	vec[0] = 1; vec[8] = -1;
	r->set(vec);

	st.set_resource(r);
	st.update();

	std::cout << "result   " << st.result() << std::endl;
	std::cout << "jacobian " << st.task_jacobian() << std::endl;

	SensorTransformPtr s(
		make_ApplyOperationSensorTransform(
			id,
			std::bind2nd(multiplies<FloatVector, double>(), 1.3),
			std::bind2nd(multiplies<FloatMatrix, double>(), 1.4)
		)
	);
}

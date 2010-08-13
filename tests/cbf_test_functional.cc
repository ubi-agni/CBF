#include <cbf/functional.h>
#include <cbf/identity_transform.h>
#include <cbf/dummy_resource.h>

#include <iostream>
#include <boost/numeric/ublas/io.hpp>

using namespace CBF;

int main() {
	MapGenericBlockWiseSensorTransformOperation
		<std::negate<FloatVector>, std::negate<FloatMatrix> >
			neg(3);

	SensorTransformPtr id(new IdentitySensorTransform(9));	

	OperationSensorTransform<
		MapGenericBlockWiseSensorTransformOperation<
			std::negate<FloatVector>, std::negate<FloatMatrix> 
		> 
	> st(neg, id);

	DummyResourcePtr r(new DummyResource(9));

	FloatVector vec(9);
	vec[0] = 1; vec[8] = -1;
	r->set(vec);

	st.set_resource(r);
	st.update();

	std::cout << "result   " << st.result() << std::endl;
	std::cout << "jacobian " << st.task_jacobian() << std::endl;
}

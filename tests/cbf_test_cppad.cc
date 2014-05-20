// for some reason, the include order is important
#include <cbf/cppad_sensor_transform.h>
#include <cbf/primitive_controller.h>
#include <cbf/dummy_reference.h>
#include <cbf/dummy_resource.h>
#include <cbf/square_potential.h>
#include <cbf/combination_strategy.h>
#include <cbf/generic_transform.h>

using namespace CBF;

int main() {
	ADFloatVector x(2);
	x[0] = 0; x[1] = 1;
	CppAD::Independent(x);

	ADFloatVector y(1);
	y[0] = CppAD::cos(x[0]) + 5.0 * CppAD::sin(x[1]);

	CppAD::ADFun<Float> f(x,y);

	CppADSensorTransformPtr s(new CppADSensorTransform(f, f.Range(), f.Domain()));

	FloatVector res(2);
	res[0] = res[1] = 0;

	s->update(res);

	std::cout << s->result() << std::endl;
	std::cout << s->task_jacobian() << std::endl;
	assert (s->result()(0) == 1.0);
	assert (s->task_jacobian()(0,0) == 0.0 && s->task_jacobian()(0,1) == 5.0);

	PrimitiveControllerPtr c(new PrimitiveController(
		0.1,
		std::vector<ConvergenceCriterionPtr>(),
		DummyReferencePtr(new DummyReference(1)),
		PotentialPtr(new SquarePotential(1,1)),
		s,
		EffectorTransformPtr(new GenericEffectorTransform(1,2)),
		std::vector<SubordinateControllerPtr>(),
		CombinationStrategyPtr(new AddingStrategy()),
		ResourcePtr(new DummyResource(res))
	));

	for (unsigned int n = 0; n < 1000; ++n)
		{ c->step(); }
}

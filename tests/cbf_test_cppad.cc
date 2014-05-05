#include <cbf/cppad_sensor_transform.h>
#include <cbf/primitive_controller.h>
#include <cbf/dummy_reference.h>
#include <cbf/dummy_resource.h>
#include <cbf/square_potential.h>
#include <cbf/combination_strategy.h>
#include <cbf/generic_transform.h>

#include <iostream>

int main() {
	using CppAD::AD;

	std::vector<AD<double> > x(2);
	x[0] = 0; x[1] = 1;
	CppAD::Independent(x);

	std::vector<AD <double> > y(1);
	y[0] = CppAD::cos(x[0]+1.2) + 5.0 * CppAD::sin(x[1]);

	CppAD::ADFun<double> f(x,y);

	CBF::CppADSensorTransformPtr s(new CBF::CppADSensorTransform(f, 1, 2));

	CBF::FloatVector res(2);
	res[0] = res[1] = 1;

	s->update(res);

	std::cout << s->result() << std::endl;
	std::cout << s->task_jacobian() << std::endl;

	CBF::PrimitiveControllerPtr c(new CBF::PrimitiveController(
		0.1,
		std::vector<CBF::ConvergenceCriterionPtr>(),
		CBF::DummyReferencePtr(new CBF::DummyReference(1)),
		CBF::PotentialPtr(new CBF::SquarePotential(1,1)),
		s,
		CBF::EffectorTransformPtr(new CBF::GenericEffectorTransform(1,2)),
		std::vector<CBF::SubordinateControllerPtr>(),
		CBF::CombinationStrategyPtr(new CBF::AddingStrategy()),
		CBF::ResourcePtr(new CBF::DummyResource(res))
	));

	for (unsigned int n = 0; n < 1000; ++n)
		{ c->step(); }
}

#include <cbf/cppad_sensor_transform.h>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>

int main() {
	using CppAD::AD;

	boost::numeric::ublas::vector<AD<double> > x(1);
	x[0] = 0;
	CppAD::Independent(x);

	boost::numeric::ublas::vector<AD<double> > y(1);
	y[0] = CppAD::sin(x[0]);

	CppAD::ADFun<double> f(x,y);

	boost::numeric::ublas::vector<double> x_p(1);
	for (unsigned int i = 0; i < 100; ++i) {
		x_p[0] = (i/100.0) * (2.0 * M_PI);
		std::cout << i << " " << x_p[0] << " " << f.Jacobian(x_p)[0] << " "  <<  std::endl;
	}

	CBF::CppADSensorTransformPtr s(new CBF::CppADSensorTransform(f, 1,1));

	s->update(CBF::FloatVector(1));

	std::cout << s->result() << std::endl;
	std::cout << s->task_jacobian() << std::endl;
}
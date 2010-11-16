#include <cbf/cppad_sensor_transform.h>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>

int main() {
	using CppAD::AD;

	boost::numeric::ublas::vector<AD<double> > x(2);
	x[0] = 0;
	x[1] = 1;
	CppAD::Independent(x);

	boost::numeric::ublas::vector<AD<double> > y(1);
	y[0] = CppAD::cos(x[0]+1.2) + 5.0 * CppAD::sin(x[1]);

	CppAD::ADFun<double> f(x,y);

	boost::numeric::ublas::vector<double> x_p(2);
	for (unsigned int i = 0; i < 100; ++i) {
		x_p[0] = (i/100.0) * (2.0 * M_PI);
		x_p[1] = 0;
		std::cout << i << " " << x_p[0] << " " << f.Jacobian(x_p)[0] << " "  <<  std::endl;
	}

	CBF::CppADSensorTransformPtr s(new CBF::CppADSensorTransform(f, 1, 2));

	CBF::FloatVector res(2);
	res[0] = res[1] = 1;

	s->update(res);

	std::cout << s->result() << std::endl;
	std::cout << s->task_jacobian() << std::endl;
}
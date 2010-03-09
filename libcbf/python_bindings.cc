#include <boost/python.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/noncopyable.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <cbf/controller.h>
#include <cbf/primitive_controller.h>
#include <cbf/resource.h>
#include <cbf/generic_transform.h>
#include <cbf/orientation_potentials.h>
#include <cbf/potential.h>
#include <cbf/sensor_transform.h>

#include <iostream>
#include <vector>

using namespace boost::python;
using namespace boost::numeric;

struct test {
	void operator()(int i) {
		std::cout << "jhejkf| " << i << std::endl;
	}

	void exec() {
		std::cout << "jhejkf|" << std::endl;
	}
};


BOOST_PYTHON_MODULE(pycbf)
{
	CBF::Float &(CBF::ublas::vector<CBF::Float>::*f)(size_t) = &CBF::ublas::vector<CBF::Float>::operator();

	//def ("create", CBF::create);

	class_<test>("testp")
		.def("ugh", &test::operator())
	;

	class_<CBF::ublas::matrix<CBF::Float> >("matrix", init<size_t, size_t>())
	;

	class_<CBF::ublas::vector<CBF::Float> >("vector", init<size_t>())
		.def("at", f, return_value_policy<return_by_value>())
		//.def("at", f, return_internal_reference<1>())
		//.def("at", &ublas::vector<CBF::Float>::operator(), return_internal_reference<>())

		//.def("size", &ublas::vector<CBF::Float>::size)
	;

	class_<CBF::Resource, CBF::ResourcePtr, boost::noncopyable >("Resource", no_init);

	//class_<CBF::DummyResource, CBF::DummyResourcePtr, bases<CBF::Resource> >("DummyResource", init<unsigned int>());

	class_<CBF::Controller, CBF::ControllerPtr>("Controller")
		.def("step", &CBF::Controller::step)
		.def("finished", &CBF::Controller::finished)
	;

	class_<CBF::PrimitiveController, bases<CBF::Controller>, CBF::PrimitiveControllerPtr>("PrimitiveController")
		.def("step", &CBF::PrimitiveController::step)
		.def("finished", &CBF::PrimitiveController::finished)
	;
}

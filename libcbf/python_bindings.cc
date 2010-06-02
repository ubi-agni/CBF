/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <boost/python.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/noncopyable.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <cbf/controller.h>
#include <cbf/primitive_controller.h>
#include <cbf/resource.h>
#include <cbf/generic_transform.h>
#include <cbf/potential.h>
#include <cbf/sensor_transform.h>

#include <iostream>
#include <vector>

using namespace boost::python;
using namespace boost::numeric;

struct test {
	void operator()(int i) {
		std::cout << "jhejkf1| " << i << std::endl;
	}

	void exec() {
		std::cout << "jhejkf2|" << std::endl;
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

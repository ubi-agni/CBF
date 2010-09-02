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

#include <cbf/plugin_pool.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/xsd_error_handler.h>

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <fstream>

#include <memory>

#include <cbf/schemas.hxx>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
	po::options_description options_description("Allowed options");
	options_description.add_options() 
		("help", "produce help message")
		("sleep-time", po::value<unsigned int>(), "time to sleep between cycles in milliseconds")
		("steps", po::value<unsigned int>(), "run exact number of steps. 0 means: never stop")
		("control-basis", po::value<std::string>(), "XML file containing controller specification(s)")
		("controller", po::value<std::vector<std::string> >(), "Name of a controller to run (can be used more than once)")
		;

	po::variables_map variables_map;

	po::store(
		po::parse_command_line(
			argc, 
			argv, 
			options_description
		), 
		variables_map
	);

	po::notify(variables_map);

	if (variables_map.count("help")) {
		std::cout << options_description << std::endl;
		return(EXIT_SUCCESS);
	}

	//! sleep time in microseconds
	unsigned int sleep_time = 0;

	if (variables_map.count("sleep-time"))
		sleep_time = variables_map["sleep-time"].as<unsigned int>();

	if (!variables_map.count("control-basis")) {
		std::cout << "No control basis specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	if (!variables_map.count("controller")) {
		std::cout << "No controller name specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	std::string control_basis_name = 
		variables_map["control-basis"].as<std::string>();

	std::vector<std::string> controller_names = 
		variables_map["controller"].as<std::vector<std::string> >();

	CBF::XSDErrorHandler err_handler;

	std::auto_ptr<ControlBasisType> cbt
		(::ControlBasis
			(control_basis_name, err_handler, xml_schema::flags::dont_validate));

	CBF::ControlBasisPtr cb(new CBF::ControlBasis(*cbt));

	for (
		unsigned int i = 0, len = controller_names.size();
		i < len;
		++i
	) {
		if (cb->controllers().find(controller_names[i]) == cb->controllers().end())
			throw std::runtime_error("Controller name not found in control basis");

		if (variables_map.count("steps")) {
			for (
				unsigned int step = 0, steps = variables_map["steps"].as<unsigned int>(); 
				(steps == 0)  || step < steps; 
				++step
			)
				{ cb->controllers()[controller_names[i]]->step(); usleep((long long int)sleep_time * 1000); }
		} else {
			while (cb->controllers()[controller_names[i]]->step() == false) {
				usleep(sleep_time);
			}
		}
	}
	return EXIT_SUCCESS;
}


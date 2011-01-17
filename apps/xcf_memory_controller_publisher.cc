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

#include <cbf/config.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/debug_macros.h>
#include <cbf/xsd_error_handler.h>

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <fstream>

#include <memory>

#include <cbf/schemas.hxx>

#include <boost/program_options.hpp>
#include <Memory/Interface.hpp>

namespace po = boost::program_options;
namespace mi = memory::interface;

int main(int argc, char *argv[]) {
	po::options_description options_description("Allowed options");
	options_description.add_options() 
		(
			"help", 
			"produce help message"
		)
		(
			"control-basis", 
			po::value<std::string>(), 
			"XML file containing controller specification(s)"
		)
		(
			"controller", 
			po::value<std::string>(), 
			"Name of a controller to publish"
		)
		(
			"active-memory", 
			po::value<std::string>(), 
			"Name of the active-memory on which to publish"
		)
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

	if (!variables_map.count("active-memory")) {
		std::cout << "No active-memory specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	std::string control_basis_name = 
		variables_map["control-basis"].as<std::string>();

	std::string controller_name = 
		variables_map["controller"].as<std::string>();

	std::string active_memory_name = 
		variables_map["active-memory"].as<std::string>();

	CBF::XSDErrorHandler err_handler;

	CBF_DEBUG("parsing XML");
	try {
		mi::MemoryInterface::pointer memoryInterface(mi::MemoryInterface::getInstance(active_memory_name));

		std::auto_ptr<CBFSchema::ControlBasis> cbt
			(CBFSchema::ControlBasis_
				(control_basis_name, err_handler, xml_schema::flags::dont_validate));

		CBF::ControlBasisPtr cb(new CBF::ControlBasis(*cbt));

		if (cb->controllers().find(controller_name) == cb->controllers().end())
			throw std::runtime_error("Controller name not found in control basis");

		CBFSchema::XCFMemoryRunController v(controller_name, cbt);
		std::ostringstream s;
		CBFSchema::XCFMemoryRunController_ (s, v);
		memoryInterface -> insert(s.str());
		
	} catch (const xml_schema::exception& e) {
		std::cerr << "Error during parsing:" << std::endl;
		std::cerr << e << std::endl;
	}

	return EXIT_SUCCESS;
}


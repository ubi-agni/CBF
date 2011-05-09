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
#include <cbf/namespace.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/debug_macros.h>
#include <cbf/xsd_error_handler.h>
#include <cbf/object_list.h>
#include <cbf/xml_object_factory.h>

#ifdef CBF_HAVE_QT
	#include <QtGui/QApplication>
#endif

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
		(
			"help", 
			"produce help message"
		)
		(
			"sleep-time", 
			po::value<unsigned int>(), 
			"time to sleep between cycles in milliseconds"
		)
		(
			"steps", 
			po::value<unsigned int>(), 
			"run exact number of steps. 0 means: never stop"
		)
		(
			"object", 
			po::value<std::vector<std::string> >(), 
			"XML file containing object specification(s) (including e.g. controllers)"
		)
		(
			"controller", 
			po::value<std::string>(), 
			"Name of a controller to run"
		)
		(
			"verbose",
			po::value<unsigned int>(),
			"Verbosity level"
		)
#ifdef CBF_HAVE_QT
		(
			"qt-main-loop",
			po::value<bool>(),
			"Provide Qt Mainloop and process events"
		)
#endif
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

	if (!variables_map.count("object")) {
		std::cout << "No XML files with object descriptions provided" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	if (!variables_map.count("controller")) {
		std::cout << "No controller name specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	std::vector<std::string> object_names = 
		variables_map["object"].as<std::vector<std::string> >();

	std::string controller_name = 
		variables_map["controller"].as<std::string>();

	CBF::XSDErrorHandler err_handler;

#ifdef CBF_HAVE_QT
	bool qt_support = false;
	if (variables_map.count("qt-main-loop")) {
		qt_support = variables_map["qt-main-loop"].as<bool>();
	}

	QApplication *app;
	if (qt_support) {
		app = new QApplication(argc, argv);
	}
#endif

	CBF_DEBUG("parsing XML");
	try {
		CBF::ObjectNamespacePtr object_namespace(new CBF::ObjectNamespace);

		for (unsigned int i = 0; i < object_names.size(); ++i) {
			CBF_DEBUG("loading control basis: " << object_names[i]);
			std::auto_ptr<CBFSchema::Object> cbt
				(CBFSchema::Object_
					(object_names[i], err_handler, xml_schema::flags::dont_validate));

			// CBF::ObjectPtr cb(new CBF::Object(*cbt, object_namespace));
			CBF::ObjectPtr cb = CBF::XMLObjectFactory::instance()->create<CBF::Object>(*cbt, object_namespace);
		}

		CBF::ControllerPtr controller = object_namespace->get<CBF::Controller>(controller_name);

		if (variables_map.count("steps")) {
			for (
				unsigned int step = 0, steps = variables_map["steps"].as<unsigned int>(); 
				(steps == 0)  || step < steps; 
				++step
			) { 
				if (variables_map.count("verbose"))
					{ std::cout << "steps" << std::endl; }

				controller->step(); 
				usleep((long long int)sleep_time * 1000); 
				#ifdef CBF_HAVE_QTobject_namespace
					if (qt_support) QApplication::processEvents();
				#endif
			}
		} else {
			while (controller->step() == false) {
				if (variables_map.count("verbose"))
					{ std::cout << "step" << std::endl; }

				usleep(sleep_time * 1000);
				#ifdef CBF_HAVE_QT
					if (qt_support) QApplication::processEvents();
				#endif
			}
		}
	} catch (const xml_schema::exception& e) {
		std::cerr << "Error during parsing:" << std::endl;
		std::cerr << e << std::endl;
	}

	return EXIT_SUCCESS;
}


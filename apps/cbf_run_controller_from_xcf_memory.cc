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


    Copyright 2011 Viktor Richter
*/

#include <cbf/config.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/debug_macros.h>
#include <cbf/xsd_error_handler.h>

#include <xcf_memory_run_controller.h>
#include <cbf_run_controller.h>

#ifdef CBF_HAVE_QT
	#include <QtGui/QApplication>
#endif

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <fstream>

#include <boost/program_options.hpp>


namespace po = boost::program_options;

const unsigned int SLEEP_TIME = 0;
const unsigned int STEPS = 0;

int main(int argc, char *argv[]) {
	po::options_description options_description("Allowed options");
	options_description.add_options() 
		(
			"help", 
			"produce help message"
		)
		(
			"cname", 
			po::value<std::string>(), 
			"Name of the XCFMemoryRunController to listen to."
		)
		(
			"memory", 
			po::value<std::string>(), 
			"Name of the active_memory to connect to"
		)
		(
			"nlevel",
			po::value<unsigned int>(),
			"Notification Level: 0 = nothing, 1 = only error, 2 = only info, 3 = all(default)"
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

	std::string run_controller_name;
	if (!variables_map.count("cname")) {
		std::cout << "No XCFMemoryRunController name specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	} else {
		run_controller_name = variables_map["cname"].as<std::string>();
	}

	std::string active_memory_name;
	if (!variables_map.count("memory")) {
		std::cout << "No active-memory name specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	} else {
		active_memory_name = variables_map["memory"].as<std::string>();
	}

	unsigned int notification_level = 3;
		if (variables_map.count("nlevel")) {
			notification_level = variables_map["nlevel"].as<unsigned int>();
		}
	CBF::XCFMemoryRunController::NotificationLevel nlvl = CBF::XCFMemoryRunController::NOTHING;
	switch (notification_level) {
	case 1 : 
		nlvl = CBF::XCFMemoryRunController::ERROR;
		break;
	case 2 : 
		nlvl = CBF::XCFMemoryRunController::INFO;
		break;
	case 3 : 
		nlvl = CBF::XCFMemoryRunController::ALL;
		break;
	}

	unsigned int verbosity_level = 0;
		if (variables_map.count("verbose")) {
			verbosity_level = variables_map["verbose"].as<unsigned int>();
		}

	#ifdef CBF_HAVE_QT
		bool qt_support = false;
		if (variables_map.count("qt-main-loop")) {
			qt_support = variables_map["qt-main-loop"].as<bool>();
		}
	#endif

	#ifdef CBF_HAVE_QT
		QApplication *app;
		if (qt_support) {
			app = new QApplication(argc, argv);
		}
	#endif

	CBF_DEBUG("creating XCFMemoryRunController");
	/*
	* Creating an XCFMemoryRunController with all the options that were set.
	* after that this thread goes to sleep.
	*/
	CBF::XCFMemoryRunController controller(run_controller_name, active_memory_name,
				nlvl, SLEEP_TIME, STEPS, verbosity_level
				#ifdef CBF_HAVE_QT
					,qt_support
				#endif
					);

	CBF_DEBUG("waiting...");
	while(true){
		// we just go to sleep. controller will run everything 
		usleep(10000 * 10000);
	}

	CBF_DEBUG("Quitting with success");
	return EXIT_SUCCESS;
	}


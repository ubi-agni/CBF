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


    Copyright 20011 Viktor Richter
*/

#include <cbf/config.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/xsd_error_handler.h>
#include <cbf/schemas.hxx>

#include <cbf_q_xcf_memory_run_controller_operator.h> 

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <fstream>

#include <memory>

#include <boost/program_options.hpp>

#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>


namespace po = boost::program_options;

int main(int argc, char *argv[]) {
	po::options_description options_description("Allowed options");
	options_description.add_options() 
		(
			"help", 
			"produce help message"
		)
		(
			"runcontrollername", 
			po::value<std::string>(), 
			"The name of the XCFMemoryRunContoller to operate"
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

	if (!variables_map.count("active-memory")) {
		std::cout << "No active-memory specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	// getting the name of the actice_memory
	std::string active_memory_name = 
		variables_map["active-memory"].as<std::string>();

	if (!variables_map.count("runcontrollername")) {
		std::cout << "No run-controller-name specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	// getting the name of the run_controller
	std::string run_controller_name = 
		variables_map["runcontrollername"].as<std::string>();

	//Initializing the QApplication.
	QApplication *app = new QApplication(argc, argv);

	// initializing an XcfMemoryRunControllerOperator so we can connect it to buttons.
	XcfMemoryRunControllerOperator op(active_memory_name, run_controller_name);

	//Initializing the Applictaions main-window.
	QWidget *window = new QWidget();
	QPushButton add("add");
	QPushButton execute("execute");
	QPushButton stop("stop");
	QPushButton sleep_time("sleep time");
	QPushButton steps("steps");
	QPushButton load("load");
	QPushButton quit("quit");

	// connecting buttons to the corresponding slots.
	QObject::connect(&add, SIGNAL(clicked()), &op, SLOT(add_control_basis()));
	QObject::connect(&execute, SIGNAL(clicked()), &op, SLOT(execute()));
	QObject::connect(&stop, SIGNAL(clicked()), &op, SLOT(stop()));
	QObject::connect(&sleep_time, SIGNAL(clicked()), &op, SLOT(set_time()));
	QObject::connect(&steps, SIGNAL(clicked()), &op, SLOT(set_steps()));
	QObject::connect(&load, SIGNAL(clicked()), &op, SLOT(load_controllers()));
	QObject::connect(&quit, SIGNAL(clicked()), app, SLOT(quit()));

	// layouting the main window
	QVBoxLayout *windowLayout = new QVBoxLayout(window);
	windowLayout -> addWidget(&add);
	windowLayout -> addWidget(&load);
	windowLayout -> addWidget(&execute);
	windowLayout -> addWidget(&stop);
	windowLayout -> addWidget(&sleep_time);
	windowLayout -> addWidget(&steps);
	windowLayout -> addWidget(&quit);

	window -> setLayout(windowLayout);
	window -> show();

	//Starting the Application
	app -> exec();
}

void XcfMemoryRunControllerOperator::add_control_basis(){
	// getting the name of the xml file.
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "../doc/examples/xml/", tr("XML (*.xml)"));
	std::string control_basis_name = fileName.toStdString();

	CBF::XSDErrorHandler err_handler;

	try {
		// trying to parse the file as a control_basis
		std::auto_ptr<CBFSchema::ControlBasis> cbt
			(CBFSchema::ControlBasis_
				(control_basis_name, err_handler, xml_schema::flags::dont_validate));

		//CBF_DEBUG("checking if control_basis is compilable");
		//CBF::ControlBasisPtr cb(new CBF::ControlBasis(*cbt));

		// creating the XCFMemoryRunControllerAdd document.
		CBFSchema::XCFMemoryRunControllerAdd v(m_RunControllerName);
		// setting the control_basis
		v.ControlBasis(cbt);

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerAdd_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());

	} catch (const xml_schema::exception& e) {
		std::cerr << "Error during parsing:" << std::endl;
		std::cerr << e << std::endl;
	}
}

void XcfMemoryRunControllerOperator::set_time(){
	// getting the new sleep time.
	bool ok;
	int i = QInputDialog::getInt(this, tr("enter new sleep time (ms)"),
			tr("sleep time:"), 0, 0, 2147483647, 100, &ok);

	if (ok){
		// creating the XCFMemoryRunControllerOptions document
		CBFSchema::XCFMemoryRunControllerOptions v(m_RunControllerName);
		// setting the sleep-time
		v.SleepTime(i);

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerOptions_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());		
	}
}

void XcfMemoryRunControllerOperator::set_steps(){
	// getting the new step count.
	bool ok;
	int i = QInputDialog::getInt(this, tr("enter new step count"),
			tr("steps:"), 0, 0, 2147483647, 100, &ok);

	if (ok){
		// creating the XCFMemoryRunControllerOptions document
		CBFSchema::XCFMemoryRunControllerOptions v(m_RunControllerName);
		// setting the step count
		v.Steps(i);

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerOptions_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());		
	}
}

void XcfMemoryRunControllerOperator::execute(){
	// getting the name of the controller to execute.
	bool ok;
	QString text = QInputDialog::getText(this, tr("enter controller name to execute"),
				tr("controller name:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty()){
		// creating the XCFMemoryRunControllerExecute document with the controller name.
		CBFSchema::XCFMemoryRunControllerExecute v(m_RunControllerName, text.toStdString());

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerExecute_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());
	}
}

void XcfMemoryRunControllerOperator::stop(){
	// creating a XCFMemoryRunControllerStop.
	CBFSchema::XCFMemoryRunControllerStop v(m_RunControllerName);
	std::ostringstream s;
	CBFSchema::XCFMemoryRunControllerStop_ (s, v);
	// sending the document to the active_memory
	m_MemoryInterface -> insert(s.str());
}

void XcfMemoryRunControllerOperator::load_controllers(){
	// getting the names of the controllers to load.
	bool ok;
	QString text = QInputDialog::getText(this, tr("enter controller names divided by space"),
				tr("controller names:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty()){
		// splitting the string to a list of controller names.
		QStringList controllers = text.split(" ", QString::SkipEmptyParts);
		
		// creating the XCFMemoryRunControllerLoadControllers document.
		CBFSchema::XCFMemoryRunControllerLoadControllers v(m_RunControllerName);

		// adding all controller names to the document
		for(QStringList::const_iterator it = controllers.begin();
			it != controllers.end(); it++)
		{
			v.ControllerName().push_back((*it).toStdString());
		}

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerLoadControllers_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());
	}
}


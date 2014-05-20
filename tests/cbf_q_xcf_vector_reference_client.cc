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


    Copyright 2009, 2010 Viktor Richter
*/


#include "cbf_q_xcf_vector_reference_client.h"

#include <cbf/types.h>
#include <xcf/RemoteServer.hpp>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <cbf/exceptions.h>
#include <cbf/xml_factory.h>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory>

#include <QtGui/QScrollArea>
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QDialog>

#include<QtGui/QTableWidget>

/** 
	@file cbf_q_xcf_vector_reference_client.cc
	@brief The implementation of the cbf_q_xcf_reference_client application
	@author Viktor Richter
 */


/** 
	@brief Initializes an instance of the CBF::Q_XCF_VECTOR_REFERENCE_CLIENT::Connection_dispatcher.
 */
int main(int argc, char *argv[]){
  new CBF::ConnectionDispatcher(argc, argv);
  return 0;
}

namespace CBF {
/** 
	@brief Starts an QDialog with the passed Text and an OK button.
 */
void showDialog(QString text, QWidget *parent=0){
	//open a QDialog
	QDialog dialog(parent);
	QGridLayout dialogLayout(&dialog);

	//Adding a Label the the pased text to the dialog.
	QLabel message(text, &dialog);
	message.setWordWrap(true);
	message.setAlignment(Qt::AlignCenter);
	dialogLayout.addWidget(&message, 0, 0, 1, 3, 0);

	//Adding an OK-button to the dialog, connecting it to the reject()-slot of the dialog.
	QPushButton okay("OK", &dialog);
	QObject::connect(&okay, SIGNAL(clicked()), &dialog, SLOT(reject()));
	dialogLayout.addWidget(&okay, 1, 1, 1, 1, 0);	

	//Setting the layout of the dialog and starting it.
	dialog.setLayout(&dialogLayout);
	dialog.exec();
}


ConnectionDispatcher::ConnectionDispatcher(int argc, char *argv[]){

	//Initializing a vector to store the Connection_manager-tabs in.
	tabs = new std::vector<ConnectionManager*>;

	//Initializing the QApplication.
	QApplication *app = new QApplication(argc, argv);

	//Initializing the Applictaions main-window.
	window = new QTabWidget;

	//Creating the connect QWidget in which the serverconnections will be build up.
	QWidget *connectWindow = new QWidget(window);
	QGridLayout *connectWindowLayout = new QGridLayout();
	connectWindow -> setLayout(connectWindowLayout);

	//Filling the Widget with controlls.
	QLabel *label = new QLabel("Insert the server address here:", connectWindow);
	label -> setFixedHeight(label -> height());
	connectWindowLayout -> addWidget(label, 0, 0, 1, 3);

	lineedit = new QLineEdit(connectWindow);
	connectWindowLayout -> addWidget(lineedit, 1, 0, 1, 2);

	QPushButton *connectbutton = new QPushButton("Initialize a new connection", connectWindow);
	QObject::connect(connectbutton, SIGNAL(clicked()), this, SLOT(connect()));
	connectWindowLayout -> addWidget(connectbutton, 1, 2, 1, 1);

	QPushButton *quitbutton = new QPushButton("Quit", connectWindow);
	QObject::connect(quitbutton, SIGNAL(clicked()), app, SLOT(quit()));
	connectWindowLayout -> addWidget(quitbutton, 2, 0, 1, 3);


	//Adding the connect-widget as a tab to the main-window.
	window -> addTab(connectWindow, "Main");
	window -> show();

	//Setting minimum size and focus
	window -> setMinimumSize(PROG_MIN_WIDTH, PROG_MIN_HEIGTH);
	lineedit -> setFocus(Qt::OtherFocusReason);
	
	//Connecting the returnPressed signal of the QLineEdit with the click on the connectbutton.
	QObject::connect(lineedit, SIGNAL(returnPressed()), connectbutton, SLOT(animateClick()));
	
	//Start connections for first command line argument.
	if(argc > 1){
		lineedit -> setText(argv[1]);
		connectbutton -> animateClick();
		window -> setCurrentIndex(0);
	}

	//Starting the Application
	app -> exec();
}


void ConnectionDispatcher::connect(){
	//Getting the text from the QlineEdit, which should hold the Servername.
	std::string input = (lineedit -> text()).toStdString();
	if(input.length()>0){
	//If a servername was entered.
		try{
			CBF_DEBUG("creating remote server object");

			//Create a RemoteServerPtr with the entered servername.
			XCF::RemoteServerPtr _remoteServer = XCF::RemoteServer::create(input.c_str());
			//Open a connection_manager-tab, add and show it.
			ConnectionManager *new_tab = 
				new ConnectionManager(window, _remoteServer,  input);
			window -> addTab(new_tab, input.c_str());
			tabs -> push_back(new_tab);
			window -> setCurrentWidget(new_tab);
		} catch(const XCF::ServerNotFoundException &e){
			showDialog("Server not found.", window);
		} catch(const XCF::InitializeException &e){
			showDialog("Problem while initializing the RemoteServer object.", window);
		}
	}
}


ConnectionManager::ConnectionManager(QWidget *parent, XCF::RemoteServerPtr _remoteServer, std::string input):
	QWidget(parent)
{
	//Initialize a vector to store the QDoubleSpinboxes in.
	spinboxes = new std::vector<QDoubleSpinBox*>;
	//Initialize a vector to store the Labels for the current values in.
	currentValueLabels = new std::vector<QLabel*>;

	//Set the local RemoteServerPtr variable to the passed Server.
	this -> _remoteServer = _remoteServer;

	//Creating timer for the update function
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(loadRemoteValues()));

	//Setting the pointer of the combobox from the 'set spinboxes to' dialog to null.
	comboSetSpinboxes = NULL;

	//Try to get the dimension from the server.
	try{
		std::string dim_string;
		_remoteServer->callMethod("get_dimension", "", dim_string);
		CBF_DEBUG("dimension_xml: " << dim_string);

		std::istringstream vv_stream(dim_string);

		std::auto_ptr<CBFSchema::Vector> dim_v = 
			CBFSchema::Vector_(vv_stream, xml_schema::flags::dont_validate);

		CBF::ObjectNamespacePtr object_namespace(new CBF::ObjectNamespace);

		CBF::FloatVector dim_vv = *CBF::XMLFactory<FloatVector>::instance()->create(*dim_v, object_namespace);
		CBF_DEBUG("dim_vv: " << dim_vv);

		dim = dim_vv[0];
	//If an Exception occurs the connection will be disabled, the tab closed and an error message will pop up.
	} catch (const XCF::ServerNotFoundException &e){
		showDialog("The server could not be localised, this connection will be closed.", this);
		disconnect();
	} catch (const XCF::CommunicationException &e){
		showDialog("There is a server communication problem, this connection will be closed.", this);
		disconnect();
	} catch (const XCF::UserException &e){
		showDialog("An Exception occured in the remotely called usercode, "
					"this connection will be closed.", this);
		disconnect();
	} catch (const XCF::NotActivatedException &e){
		showDialog("The run-method of the Server has not been called (yet), "
					"this connection will be closed.", this);
		disconnect();
	}

	//Setting up the layout of the tab
	QGridLayout *layout = new QGridLayout(this);
	this -> setLayout(layout);
	
	QString connectedto = "Connected to: ";
	connectedto.append(input.c_str());

	QLabel *label = new QLabel(connectedto);
	layout -> addWidget(label, 0, 0, 1, 2);

	//Making a QWidget for QDoubleSpinboxes.
	inputWin = new QWidget(this);
	QGridLayout *inputWinLayout = new QGridLayout(inputWin);
	inputWinLayout -> setSizeConstraint(QLayout::SetNoConstraint);
	inputWin -> setLayout(inputWinLayout);

	//Adding the Spinboxes with a Label.
	spinboxes -> reserve(dim);
	currentValueLabels -> reserve(dim);
	unsigned int i;
	for (i=0; i<dim; i++){
		std::ostringstream valno;
		valno << "Value Nr. " << i+1;
		inputWinLayout -> addWidget(new QLabel(valno.str().c_str(), inputWin), i, 0, 1, 1);

		QDoubleSpinBox *spinbox = new QDoubleSpinBox(inputWin);
		spinbox -> setKeyboardTracking(false);
		spinbox -> setDecimals(SPINBOX_DECIMALS);
		spinbox -> setSingleStep(SPINBOX_STEP);
		spinbox -> setMinimum(SPINBOX_MIN);
		spinbox -> setMaximum(SPINBOX_MAX);
		(*spinboxes)[i] = spinbox;
		inputWinLayout -> addWidget(spinbox, i, 1, 1, 3);

		QLabel *currentValLabel = new QLabel(inputWin);
		(*currentValueLabels)[i] = currentValLabel;
		inputWinLayout -> addWidget(currentValLabel, i, 4, 1, 1);
	}
	
	//Making the Widget scrollable.
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea -> setWidget(inputWin);
	scrollArea -> setAlignment(Qt::AlignCenter);
	layout -> addWidget(scrollArea, 1, 0, 1, 2);
	
	//Creating and adding the option-widget.
	makeOptionsWidget();
	layout -> addWidget(optionsTabWidget, 2, 0, 1, 2);

	//Adding a checkbox to show and hide the options-widget.
	optionsCheckBox = new QCheckBox("show options", this);
	layout -> addWidget(optionsCheckBox, 3, 0, 1, 2);

	optionsCheckBox -> setChecked(SHOW_OPTIONS);
	QObject::connect(optionsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showOptionsWidget()));
	showOptionsWidget();

	//Adding the sendButton.
	QPushButton *send = new QPushButton("Send");
	QObject::connect(send, SIGNAL(clicked()), this, SLOT(send()));	
	layout -> addWidget(send, 4, 0, 1, 1);

	//Adding the loadRemoteValuesButton.
	QPushButton *loadRemoteValues = new QPushButton("Load remote values");
	QObject::connect(loadRemoteValues, SIGNAL(clicked()), this, SLOT(loadRemoteValues()));	
	layout -> addWidget(loadRemoteValues, 4, 1, 1, 1);

	//Adding the setSpinboxesToButton.
	QPushButton *setSpinboxesTo = new QPushButton("Set spinboxes to:");
	QObject::connect(setSpinboxesTo, SIGNAL(clicked()), this, SLOT(setSpinboxesTo()));	
	layout -> addWidget(setSpinboxesTo, 5, 0, 1, 1);

	//Adding a ComboBox for the setSpinboxesTo function.
	comboSetSpinboxes = new QComboBox(this);
	comboSetSpinboxes -> addItem("set to current values from server");
	comboSetSpinboxes -> addItem("set to already loaded values");
	comboSetSpinboxes -> addItem("choose single value");
	comboSetSpinboxes -> setCurrentIndex(0);
	layout -> addWidget(comboSetSpinboxes, 5, 1, 1, 1);

	//Adding the disconnectButton
	QPushButton *disconnect = new QPushButton("Disconnect");
	QObject::connect(disconnect, SIGNAL(clicked()), this, SLOT(disconnect()));
	layout -> addWidget(disconnect, 6, 0, 1, 2);
}


void ConnectionManager::makeOptionsWidget(){	
	//Initializing the QTabWidget, with the options.
	optionsTabWidget = new QTabWidget(this);


	//Initializing a QWidget for the server options.
	QWidget *serverOptionsWidget = new QWidget(optionsTabWidget);
	QGridLayout *serverOptionsLayout = new QGridLayout(serverOptionsWidget);
	serverOptionsWidget -> setLayout(serverOptionsLayout);	

	//Adding a checkbox for the 'always send' option.
	alwaysSendCheckBox = new QCheckBox("Always send", serverOptionsWidget);
	alwaysSendCheckBox -> setToolTip("This option makes the programm send each change of the values in the "
						"spinboxes immediately. ");
	serverOptionsLayout -> addWidget(alwaysSendCheckBox, 0, 0, 1, 3);
	alwaysSendCheckBox -> setChecked(ALWAYS_SEND);
	QObject::connect(alwaysSendCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changeSendMode()));
	if (ALWAYS_SEND) changeSendMode();

	//Adding a checkbox for the 'always load' option.
	alwaysLoadCheckBox = new QCheckBox("Always load", serverOptionsWidget);
	alwaysLoadCheckBox -> setToolTip("This option makes the programm load the remote task position and then "
						"make the specified pause");
	serverOptionsLayout -> addWidget(alwaysLoadCheckBox, 1, 0, 1, 3);
	alwaysLoadCheckBox -> setChecked(ALWAYS_LOAD);
	QObject::connect(alwaysLoadCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changeLoadMode()));
	if (ALWAYS_LOAD) changeLoadMode();
	serverOptionsLayout -> addWidget(new QLabel("Reload every", serverOptionsWidget), 2, 0, 1, 1);

	//Adding a spinbox to change the pause time.
	reloadPauseSpinbox = new QSpinBox(serverOptionsWidget);
	reloadPauseSpinbox -> setRange(RELOAD_PAUSE_TIME_MIN, RELOAD_PAUSE_TIME_MAX);
	reloadPauseSpinbox -> setSingleStep(RELOAD_PAUSE_TIME_STEP);
	reloadPauseSpinbox -> setSuffix(" ms");
	reloadPauseSpinbox -> setValue(RELOAD_PAUSE_TIME_FIRST);
	reloadPauseSpinbox -> setKeyboardTracking(false);
	QObject::connect(reloadPauseSpinbox, SIGNAL(valueChanged(int)), this, SLOT(changeLoadPauseTime(int)));
	serverOptionsLayout -> addWidget(reloadPauseSpinbox, 2, 1, 1, 2);

	optionsTabWidget -> addTab(serverOptionsWidget, "Server");


	//The tooltip text for the decimals option.
	const char* TOOLTIP_DECIMALS = "The option 'set decimals to' changes the count of "
						"numbers after the point. It only affects the spinboxes.";
	//The tooltip text for the stepsize option.
	const char* TOOLTIP_STEP = "The option 'set stepsize to' changes the value by which the numbers "
						"in the spinboxes are increased/decreased through the up/down button.";
	//The tooltip text for the maximum value option.
	const char* TOOLTIP_MAX = "The option 'set maximum value to' changes "
						"the maximum value of the spinboxes.";
	//The tooltip text for the minimum value option.
	const char* TOOLTIP_MIN = "The option 'set minimum value to' changes "
						"the minimum value of the spinboxes.";

	//Creating an int and a double validator for the QLineEdits.
	QIntValidator *intValidator = new QIntValidator(SPINBOX_DECIMALS_MIN, SPINBOX_DECIMALS_MAX, this);
	QDoubleValidator *doubleValidator = new QDoubleValidator(this);

	//Initializing a QWidget for the Spinbox-Options
	QWidget *spinboxOptionsWidget = new QWidget(optionsTabWidget);
	QGridLayout *spinboxOptionsLayout = new QGridLayout(spinboxOptionsWidget);
	spinboxOptionsWidget -> setLayout(spinboxOptionsLayout);	
	
 	//Adding an option to change the count of decimals in the Spinboxes.
	QPushButton *decimalsButton = new QPushButton("Set decimals to:", spinboxOptionsWidget);
	decimalsButton -> setToolTip(TOOLTIP_DECIMALS);
	spinboxOptionsLayout -> addWidget(decimalsButton, 0, 0, 0);

	std::ostringstream decimals;
	decimals << SPINBOX_DECIMALS;

	decimalsLineEdit = new QLineEdit(decimals.str().c_str(), spinboxOptionsWidget);
	decimalsLineEdit -> setToolTip(TOOLTIP_DECIMALS);

	// adding an int validator to the decimalsLineEdit
	decimalsLineEdit -> setValidator(intValidator);
	
	spinboxOptionsLayout -> addWidget(decimalsLineEdit, 0, 1, 0);

	QObject::connect(decimalsLineEdit, SIGNAL(returnPressed()), decimalsButton, SLOT(animateClick()));
	QObject::connect(decimalsButton, SIGNAL(clicked()), this, SLOT(setDecimals()));

 	//Adding an option to change the stepsize in the Spinboxes.
	QPushButton *stepSizeButton = new QPushButton("Set stepsize to:", spinboxOptionsWidget);
	stepSizeButton -> setToolTip(TOOLTIP_STEP);
	spinboxOptionsLayout -> addWidget(stepSizeButton, 1, 0, 0);	

	std::ostringstream stepSize;
	stepSize << SPINBOX_STEP;

	stepSizeLineEdit = new QLineEdit(stepSize.str().c_str(), spinboxOptionsWidget);
	stepSizeLineEdit -> setToolTip(TOOLTIP_STEP);

	// adding a double validator to the stepSizeLineEdit
	stepSizeLineEdit -> setValidator(doubleValidator);

	spinboxOptionsLayout -> addWidget(stepSizeLineEdit, 1, 1, 0);

	QObject::connect(stepSizeLineEdit, SIGNAL(returnPressed()), stepSizeButton, SLOT(animateClick()));
	QObject::connect(stepSizeButton, SIGNAL(clicked()), this, SLOT(setStepSize()));

 	//Adding an option to change the minimum and maximum of the Spinboxes.
	QPushButton *maxButton = new QPushButton("Set maximum value to:", spinboxOptionsWidget);
	maxButton -> setToolTip(TOOLTIP_MAX);
	spinboxOptionsLayout -> addWidget(maxButton, 2, 0, 0);
	QPushButton *minButton = new QPushButton("Set minimum value to:", spinboxOptionsWidget);
	minButton -> setToolTip(TOOLTIP_MIN);
	spinboxOptionsLayout -> addWidget(minButton, 3, 0, 0);

	std::ostringstream min, max;
	max << SPINBOX_MAX;
	maxLineEdit = new QLineEdit(max.str().c_str(), spinboxOptionsWidget);
	maxLineEdit -> setToolTip(TOOLTIP_MAX);

	// adding a double validator to the maxLineEdit
	maxLineEdit -> setValidator(doubleValidator);

	spinboxOptionsLayout -> addWidget(maxLineEdit, 2, 1, 0);

	min << SPINBOX_MIN;
	minLineEdit = new QLineEdit(min.str().c_str(), spinboxOptionsWidget);
	minLineEdit -> setToolTip(TOOLTIP_MIN);

	// adding a double validator to the minLineEdit
	minLineEdit -> setValidator(doubleValidator);

	spinboxOptionsLayout -> addWidget(minLineEdit, 3, 1, 0);

	QObject::connect(minButton, SIGNAL(clicked()), this, SLOT(setMinValue()));
	QObject::connect(maxButton, SIGNAL(clicked()), this, SLOT(setMaxValue()));


	optionsTabWidget -> addTab(spinboxOptionsWidget, "Spinboxes");
}

void ConnectionManager::disconnect(){
	try{
		//Disconnecting from the server.
		_remoteServer -> destroy();
	} catch (...){
		showDialog("An error occured while shutting down the communication with the server.", this);
	}
	//Deleting the spinbox-vector.
	delete spinboxes;
	//Deleting the tab after the handling of the calling signal. The tab should disappear right away.
	this -> deleteLater();
}

void ConnectionManager::send(){
	//Creating the appropriate string for the server communication.
	std::stringstream vector_string;
	vector_string << "[" << dim <<"](";
	for (unsigned int i = 0; i < dim; i++) {
		//Getting the values from the spinboxes.
		vector_string << ((*spinboxes)[i] -> value());
		if (i != dim - 1) vector_string << ",";
	}
	vector_string << ")";
	std::cout << vector_string.str() << std::endl;
	
	try{
	//Trying to send the new values
		CBF_DEBUG("creating vector doc");
		CBFSchema::BoostVector v(vector_string.str());

		std::ostringstream s;
		//s << v;
		CBFSchema::Vector_ (s, v);

		CBF_DEBUG("document: " << s.str());

		std::string out;

		CBF_DEBUG("calling remote method");
		_remoteServer -> callMethod("set_reference", s.str(), out);
	//If an Exception occurs an error message will pop up.
	} catch (const XCF::ServerNotFoundException &e){
		showDialog("The server could not be localised, this connection can be closed.", this);
	} catch (const XCF::CommunicationException &e){
		showDialog("There is a server communication problem, this connection can be closed.", this);
	} catch (const XCF::UserException &e){
		showDialog("An Exception occured in the remotely called usercode, "
					"this connection can be closed.", this);
	} catch (const XCF::NotActivatedException &e){
		showDialog("The run-method of the Server has not been called (yet), "
					"this connection can be closed.", this);
	}
}

void ConnectionManager::changeSendMode(){
	if(alwaysSendCheckBox -> isChecked()){
		for (unsigned int i = 0; i < dim; i++) {
			//Connecting the valueChanged signal of the spinboxes directly to the send slot.
			QObject::connect((*spinboxes)[i],
			SIGNAL(valueChanged(double)), this, SLOT(send()));
		}
	} else {
		for (unsigned int i = 0; i < dim; i++) {
			//Disconnecting the valueChanged signal of the spinboxes from the send slot.
			QObject::disconnect((*spinboxes)[i], 
			SIGNAL(valueChanged(double)), this, SLOT(send()));
		}
	}
}

CBF::ConnectionManager::FloatVectorPtr ConnectionManager::loadCurrentPositionVector(){
	try{
		//Getting the current task position from the server.
		std::string xml_in, out;
		_remoteServer -> callMethod("get_current_task_position", out, xml_in);

		//Parsing XML for FloatVector
		CBF_DEBUG("doc: " << xml_in);
		std::istringstream s(xml_in);
		std::auto_ptr<CBFSchema::Vector> v = CBFSchema::Vector_(s, xml_schema::flags::dont_validate);
		CBF_DEBUG("create vector");
		CBF::ObjectNamespacePtr object_namespace(new CBF::ObjectNamespace);
		CBF::ConnectionManager::FloatVectorPtr currentPositionVector = 
			CBF::ConnectionManager::FloatVectorPtr(new CBF::FloatVector
					(*CBF::XMLFactory<FloatVector>::instance()
						->create(*v, object_namespace)));

		CBF_DEBUG("vector created");
		if (currentPositionVector -> size() != dim) {
			CBF_THROW_RUNTIME_ERROR(
				"Dimensions of xml vector not matching the dimension");
		}
		return currentPositionVector;
	} catch (const XCF::MethodNotFoundException &e){
		CBF_DEBUG("The Remote does not seem to provide souch a service.");
		throw;
	} catch (const std::runtime_error &e){
		CBF_DEBUG("Runtime error while parsing remote vector: " << e.what());
		CBF_DEBUG("The dimension of the remote task does not match the local saved dimension. Maybe"
			"the Task needs to be run first.");
	} catch (const XCF::CommunicationException &e){
		showDialog("There is a server communication problem, this connection can be closed.", this);
	}
	alwaysLoadCheckBox -> setChecked(false);
	changeLoadMode();
	return CBF::ConnectionManager::FloatVectorPtr();
}

void ConnectionManager::loadRemoteValues(){
	CBF::ConnectionManager::FloatVectorPtr currentPositionVector = loadCurrentPositionVector();
	if(currentPositionVector.get() != NULL){
		for (unsigned int i = 0; i < dim; i++) {
			//setting the value for every Label.
			QLabel* label = (*currentValueLabels)[i];
			std::stringstream value;
			value << (*currentPositionVector)(i);
			label -> setText(value.str().c_str());
		}
		inputWin -> resize(inputWin -> sizeHint());
	}
}

void ConnectionManager::changeLoadMode(){
	if(alwaysLoadCheckBox -> isChecked()){
		//Starting the timer
		timer -> start(reloadPauseSpinbox -> value());
	} else {
		//Stopping the timer
		timer -> stop();
	}
}

void ConnectionManager::changeLoadPauseTime(int time){
	timer -> setInterval(time);
}


void ConnectionManager::setSpinboxesTo(){
	switch(comboSetSpinboxes -> currentIndex()){
		case 0:{ //"set to current values from server" - Getting values from Server.
			CBF::ConnectionManager::FloatVectorPtr currentPositionVector = loadCurrentPositionVector();
			if(currentPositionVector.get() != NULL){
				for (unsigned int i = 0; i < dim; i++) {
					//setting the value for every spinbox.
					(*spinboxes)[i] -> setValue((*currentPositionVector)(i));
				}
				inputWin -> resize(inputWin -> sizeHint());
			}
			break;
		}
		case 1:{ //"set to already loaded values" - Getting values from Labels.
			if(((*currentValueLabels)[0] -> text()) != ""){
				for (unsigned int i = 0; i < dim; i++) {
					//setting the value for every spinbox.
					double value = ((*currentValueLabels)[i] -> text()).toDouble();
					(*spinboxes)[i] -> setValue(value);
				}
				inputWin -> resize(inputWin -> sizeHint());
			}
			break;
		}
		case 2:{ //"choose single value" - Getting value through a dialog.
			QDialog dialog(this);
			QGridLayout dialogLayout(&dialog);

			//Adding a Label with text to the dialog.
			QLabel message("Choose the value:", &dialog);
			message.setWordWrap(true);
			message.setAlignment(Qt::AlignCenter);
			dialogLayout.addWidget(&message, 0, 0, 1, 2, Qt::AlignHCenter);

			//Adding a QLineEdit the new value.
			QLineEdit inputLine(&dialog);
			QDoubleValidator *doubleValidator = new QDoubleValidator(this);
			inputLine.setValidator(doubleValidator);
			dialogLayout.addWidget(&inputLine, 1, 0, 1, 2, Qt::AlignHCenter);
			QObject::connect(&inputLine, SIGNAL(returnPressed()), &dialog, SLOT(accept()));			

			//Adding cancell and okay buttons to the dialog.
			QPushButton cancel("Cancel", &dialog);
			QObject::connect(&cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
			dialogLayout.addWidget(&cancel, 2, 0, 1, 1, Qt::AlignRight);

			QPushButton okay("Ok", &dialog);
			QObject::connect(&okay, SIGNAL(clicked()), &dialog, SLOT(accept()));
			dialogLayout.addWidget(&okay, 2, 1, 1, 1, Qt::AlignLeft);

			//Setting the layout of the dialog and starting it.
			dialog.setLayout(&dialogLayout);
			dialog.exec();
			if(dialog.result()==QDialog::Accepted && inputLine.text()!=""){
				for (unsigned int i = 0; i < dim; i++) {
					//setting the value for every spinbox.
					double value = inputLine.text().toDouble();
					(*spinboxes)[i] -> setValue(value);
				}
				inputWin -> resize(inputWin -> sizeHint());
			}
			break;
		}
	}
}

void ConnectionManager::showOptionsWidget(){
	if(optionsCheckBox -> isChecked()){
		optionsTabWidget -> show();
	} else {
		optionsTabWidget -> hide();
	}
}

void ConnectionManager::setDecimals(){
	bool ok = false;
	//Getting the int value from the decimalsLineEdit.
	int value = (decimalsLineEdit -> text().toInt(&ok));
	if(ok){ //if value is valid.
		for (unsigned int i = 0; i < dim; i++) {
			//setting the value as count of decimals for every spinbox.
			QDoubleSpinBox* spinbox = (*spinboxes)[i];
			spinbox -> setDecimals(value);;
		}
		inputWin -> resize(inputWin -> sizeHint());
	} else{ //should not happen because of validator.
		showDialog("At this place an integer value ist needed.", this);
	}
}


void ConnectionManager::setStepSize(){
	bool ok = false;
	//Getting the double value from the stepSizeLineEdit.
	double value = (stepSizeLineEdit -> text().toDouble(&ok));
	if(ok){ //if value is valid.
		for (unsigned int i = 0; i < dim; i++) {
			//setting the value as singleStep for every spinbox.
			QDoubleSpinBox* spinbox = (*spinboxes)[i];
			spinbox -> setSingleStep(value);
		}
	} else{ //should not happen because of validator.
		showDialog("At this place a double-type value ist needed.", this);
	}
}


void ConnectionManager::setMinValue(){
	bool ok = false;
	//Getting the double value from the minLineEdit.
	double value = (minLineEdit -> text().toDouble(&ok));
	bool isLess = (value < (*spinboxes)[0] -> maximum());
	if(ok){ //if value is valid.
		if(isLess){ //if value is less then the spinbox maximum.
			for (unsigned int i = 0; i < dim; i++) {
				//setting the value as minimum for every spinbox.
				QDoubleSpinBox* spinbox = (*spinboxes)[i];
				spinbox -> setMinimum(value);
			}
			inputWin -> resize(inputWin -> sizeHint());
		} else{  //should not happen because of validator.
			showDialog("The minimum value of the spinboxes must be less then the maximum value", this);
		}
	} else{ //should not happen because of validator.
		showDialog("At this place a double-type value ist needed.", this);
	}
}


void ConnectionManager::setMaxValue(){
	bool ok = false;
	//Getting the double value from the maxLineEdit.
	double value = (maxLineEdit -> text().toDouble(&ok));
	bool isGreater = (value > (*spinboxes)[0] -> minimum());
	if(ok){ //if value is valid.
		if(isGreater){ //if value is greater then the spinbox minimum.
			for (unsigned int i = 0; i < dim; i++) {
				//setting the value as maximum for every spinbox.
				QDoubleSpinBox* spinbox = (*spinboxes)[i];
				spinbox -> setMaximum(value);
			}
			inputWin -> resize(inputWin -> sizeHint());
		} else{  //should not happen because of validator.
			showDialog("The maximum value of the spinboxes must be greater then the minimum value", this);
		}
	} else{  //should not happen because of validator.
		showDialog("At this place a double-type value ist needed.", this);
	}
}
} //namespace

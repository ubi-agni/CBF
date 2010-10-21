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


#include <cbf_q_xcf_vector_reference_client.h>

#include <cbf/types.h>
#include <xcf/RemoteServer.hpp>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <boost/numeric/ublas/io.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory>

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QDialog>

/** 
	@file cbf_q_xcf_vector_reference_client.cc
	@brief The implementation of the cbf_q_xcf_reference_client application
	@author Viktor Richter
 */


/** 
	@brief Initializes an instance of the CBF::Q_XCF_VECTOR_REFERENCE_CLIENT::Connection_dispatcher.
 */
int main(int argc, char *argv[]){
  new cbf_q_xcf_vector_reference_client::Connection_dispatcher(argc, argv);
  return 0;
}

namespace cbf_q_xcf_vector_reference_client {
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


Connection_dispatcher::Connection_dispatcher(int argc, char *argv[]){

	//Initializing a vector to store the Connection_manager-tabs in.
	tabs = new std::vector<Connection_manager*>;

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
	
	//Starting the Application
	app -> exec();
}


void Connection_dispatcher::connect(){
	//Getting the text from the QlineEdit, which should hold the Servername.
	std::string input = (lineedit -> text()).toStdString();
	if(input.length()>0){
	//If a servername was entered.
		try{
			CBF_DEBUG("creating remote server object")

			//Create a RemoteServerPtr with the entered servername.
			XCF::RemoteServerPtr _remoteServer = XCF::RemoteServer::create(input.c_str());
			//Open a connection_manager-tab, add and show it.
			Connection_manager *new_tab = 
				new Connection_manager(window, _remoteServer,  input);
			window -> addTab(new_tab, input.c_str());
			tabs -> push_back(new_tab);
			window -> setCurrentWidget(new_tab);
		} catch(const XCF::ServerNotFoundException &e){
			showDialog("Server not found.", window);
		} catch(const XCF::InitializeException &e){
			showDialog("Problem while initializing the RemoteServer object.", window);
		} catch(...){
			showDialog("This should not have happened.", window);
		}
	}
}


Connection_manager::Connection_manager(QWidget *parent, 
							XCF::RemoteServerPtr _remoteServer, std::string input):
	QWidget(parent)
{
	//Initialize a vector to store the QDoubleSpinboxes in.
	spinboxes = new std::vector<QDoubleSpinBox*>;

	//Set the local RemoteServerPtr variable to the passed Server.
	this -> _remoteServer = _remoteServer;

	//Try to get the dimension from the server.
	try{
		std::string dim_string;
		_remoteServer->callMethod("get_dimension", "", dim_string);
		CBF_DEBUG("dimension_xml: " << dim_string)

		std::istringstream vv_stream(dim_string);

		std::auto_ptr<CBFSchema::Vector> dim_v = 
			CBFSchema::Vector_(vv_stream, xml_schema::flags::dont_validate);
		CBF::FloatVector dim_vv = CBF::create_vector(*dim_v);
		CBF_DEBUG("dim_vv: " << dim_vv)

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
	} catch(...){
		showDialog("An error occured while trying to get the dimension from the server, "
					"this connection will be closed.", this);
		disconnect();
	}

	//Setting up the layout of the tab
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	QString connectedto = "Connected to: ";
	connectedto.append(input.c_str());

	QLabel *label = new QLabel(connectedto);
	layout -> addWidget(label);

	//Making a QWidget for QDoubleSpinboxes.
	QWidget *inputWin = new QWidget();
	QGridLayout *inputWinLayout = new QGridLayout(inputWin);

	//Adding the Spinboxes with a Label.
	spinboxes -> reserve(dim);
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
		inputWinLayout -> addWidget(spinbox, i, 2, 1, 2);
	}	
	//Making the Widget scrollable.
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea -> setWidget(inputWin);
	layout -> addWidget(scrollArea);
	
	//Creating and adding the option-widget.
	makeOptionsWidget();
	layout -> addWidget(optionsWidget);

	//Adding a checkbox to show and hide the options-widget.
	optionsCheckBox = new QCheckBox("show options", this);
	layout -> addWidget(optionsCheckBox);

	optionsCheckBox -> setChecked(SHOW_OPTIONS);
	QObject::connect(optionsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showOptionsWidget()));
	showOptionsWidget();

	//Adding a checkbox for the 'always send' option.
	alwaysSendCheckBox = new QCheckBox("always send", this);
	alwaysSendCheckBox -> setToolTip("This option makes the programm send each change of the values in the "
						"spinboxes immediately. ");
	layout -> addWidget(alwaysSendCheckBox);
	alwaysSendCheckBox -> setChecked(ALWAYS_SEND);
	QObject::connect(alwaysSendCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changeSendMode()));
	changeSendMode();

	//Adding the sendButton.
	QPushButton *send = new QPushButton("Send");
	QObject::connect(send, SIGNAL(clicked()), this, SLOT(send()));	
	layout -> addWidget(send);

	//Adding the disconnectButton
	QPushButton *disconnect = new QPushButton("Disconnect");
	QObject::connect(disconnect, SIGNAL(clicked()), this, SLOT(disconnect()));
	layout -> addWidget(disconnect);
}


void Connection_manager::makeOptionsWidget(){	

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
	

	//Initializing the QWidget, with the options.
	optionsWidget = new QWidget(this);
	QGridLayout *optionsLayout = new QGridLayout(optionsWidget);
	optionsWidget -> setLayout(optionsLayout);	
	
 	//Adding an option to change the count of decimals in the Spinboxes.
	QPushButton *decimalsButton = new QPushButton("Set decimals to:", optionsWidget);
	decimalsButton -> setToolTip(TOOLTIP_DECIMALS);
	optionsLayout -> addWidget(decimalsButton, 0, 0, 0);

	std::ostringstream decimals;
	decimals << SPINBOX_DECIMALS;

	decimalsLineEdit = new QLineEdit(decimals.str().c_str(), optionsWidget);
	decimalsLineEdit -> setToolTip(TOOLTIP_DECIMALS);

	// adding an int validator to the decimalsLineEdit
	decimalsLineEdit -> setValidator(intValidator);
	
	optionsLayout -> addWidget(decimalsLineEdit, 0, 1, 0);

	QObject::connect(decimalsLineEdit, SIGNAL(returnPressed()), decimalsButton, SLOT(animateClick()));
	QObject::connect(decimalsButton, SIGNAL(clicked()), this, SLOT(setDecimals()));


 	//Adding an option to change the stepsize in the Spinboxes.
	QPushButton *stepSizeButton = new QPushButton("Set stepsize to:", optionsWidget);
	stepSizeButton -> setToolTip(TOOLTIP_STEP);
	optionsLayout -> addWidget(stepSizeButton, 1, 0, 0);	

	std::ostringstream stepSize;
	stepSize << SPINBOX_STEP;

	stepSizeLineEdit = new QLineEdit(stepSize.str().c_str(), optionsWidget);
	stepSizeLineEdit -> setToolTip(TOOLTIP_STEP);

	// adding a double validator to the stepSizeLineEdit
	stepSizeLineEdit -> setValidator(doubleValidator);

	optionsLayout -> addWidget(stepSizeLineEdit, 1, 1, 0);

	QObject::connect(stepSizeLineEdit, SIGNAL(returnPressed()), stepSizeButton, SLOT(animateClick()));
	QObject::connect(stepSizeButton, SIGNAL(clicked()), this, SLOT(setStepSize()));


 	//Adding an option to change the minimum and maximum of the Spinboxes.
	QPushButton *maxButton = new QPushButton("Set maximum value to:", optionsWidget);
	maxButton -> setToolTip(TOOLTIP_MAX);
	optionsLayout -> addWidget(maxButton, 2, 0, 0);
	QPushButton *minButton = new QPushButton("Set minimum value to:", optionsWidget);
	minButton -> setToolTip(TOOLTIP_MIN);
	optionsLayout -> addWidget(minButton, 3, 0, 0);

	std::ostringstream min, max;
	max << SPINBOX_MAX;
	maxLineEdit = new QLineEdit(max.str().c_str(), optionsWidget);
	maxLineEdit -> setToolTip(TOOLTIP_MAX);

	// adding a double validator to the maxLineEdit
	maxLineEdit -> setValidator(doubleValidator);

	optionsLayout -> addWidget(maxLineEdit, 2, 1, 0);

	min << SPINBOX_MIN;
	minLineEdit = new QLineEdit(min.str().c_str(), optionsWidget);
	minLineEdit -> setToolTip(TOOLTIP_MIN);

	// adding a double validator to the minLineEdit
	minLineEdit -> setValidator(doubleValidator);

	optionsLayout -> addWidget(minLineEdit, 3, 1, 0);

	QObject::connect(minButton, SIGNAL(clicked()), this, SLOT(setMinValue()));
	QObject::connect(maxButton, SIGNAL(clicked()), this, SLOT(setMaxValue()));
}


void Connection_manager::send(){
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
		CBF_DEBUG("creating vector doc")
		CBFSchema::BoostVector v(vector_string.str());

		std::ostringstream s;
		//s << v;
		CBFSchema::Vector_ (s, v);

		CBF_DEBUG("document: " << s.str())

		std::string out;

		CBF_DEBUG("calling remote method")
		_remoteServer->callMethod("set_reference", s.str(), out);
	//If an Exception occurs an error message will pop up.
	} catch (const XCF::ServerNotFoundException &e){
		showDialog("The server could not be localised, this connection will be closed.", this);
	} catch (const XCF::CommunicationException &e){
		showDialog("There is a server communication problem, this connection will be closed.", this);
	} catch (const XCF::UserException &e){
		showDialog("An Exception occured in the remotely called usercode, "
					"this connection will be closed.", this);
	} catch (const XCF::NotActivatedException &e){
		showDialog("The run-method of the Server has not been called (yet), "
					"this connection will be closed.", this);
	} catch(...){
		showDialog("An error occured while trying to send information to the server, "
					"this connection will be closed.", this);
	}
}


void Connection_manager::showOptionsWidget(){
	if(optionsCheckBox -> isChecked()){
		optionsWidget -> show();
	} else {
		optionsWidget -> hide();
	}
}


void Connection_manager::changeSendMode(){
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


void Connection_manager::disconnect(){	
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


void Connection_manager::setDecimals(){
	bool ok = false;
	//Getting the int value from the decimalsLineEdit.
	int value = (decimalsLineEdit -> text().toInt(&ok));
	if(ok){ //if value is valid.
		for (unsigned int i = 0; i < dim; i++) {
			//setting the value as count of decimals for every spinbox.
			QDoubleSpinBox* spinbox = (*spinboxes)[i];
			spinbox -> setDecimals(value);;
		}
	} else{ //should not happen because of validator.
		showDialog("At this place an integer value ist needed.", this);
	}
}


void Connection_manager::setStepSize(){
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


void Connection_manager::setMinValue(){
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
		} else{  //should not happen because of validator.
			showDialog("The minimum value of the spinboxes must be less then the maximum value", this);
		}
	} else{ //should not happen because of validator.
		showDialog("At this place a double-type value ist needed.", this);
	}
}


void Connection_manager::setMaxValue(){
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
		} else{  //should not happen because of validator.
			showDialog("The maximum value of the spinboxes must be greater then the minimum value", this);
		}
	} else{  //should not happen because of validator.
		showDialog("At this place a double-type value ist needed.", this);
	}
}
} //namespace

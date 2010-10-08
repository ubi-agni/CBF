#include <cbf_q_xcf_vector_reference_client.h>

#include <cbf/types.h>
//#include <xcf/RemoteServer.hpp>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <boost/numeric/ublas/io.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory>

int main(int argc, char *argv[]){
  new Test_xcf_reference_client_gui(argc, argv);
  return 0;
}

Test_xcf_reference_client_gui::Test_xcf_reference_client_gui(int argc, char *argv[]){

	tabs = new std::vector<Xcf_enter_remote_values_tab*>;

	app = new QApplication(argc, argv);

	window = new QTabWidget;

	connectWindow = new QWidget();
	QGridLayout *connectWindowLayout = new QGridLayout();
	connectWindow -> setLayout(connectWindowLayout);

	label = new QLabel("Insert the server address here:", connectWindow);
	label -> setFixedHeight(label -> height());
	lineedit = new QLineEdit(connectWindow);
	okaybutton = new QPushButton("Initialize a new connection", connectWindow);
	exitbutton = new QPushButton("Quit", connectWindow);

	connectWindowLayout -> addWidget(label, 0, 0, 1, 3);
	connectWindowLayout -> addWidget(lineedit, 1, 0, 1, 2);
	connectWindowLayout -> addWidget(okaybutton, 1, 2, 1, 1);
	connectWindowLayout -> addWidget(exitbutton, 2, 0, 1, 3);


	window -> addTab(connectWindow, "Main");
	window -> show();

	QObject::connect(exitbutton, SIGNAL(clicked()), this, SLOT(quit()));
	QObject::connect(okaybutton, SIGNAL(clicked()), this, SLOT(connect()));

	window -> setMinimumSize(PROG_MIN_WIDTH, PROG_MIN_HEIGTH);

	app -> exec();
}

void Test_xcf_reference_client_gui::connect(){
	std::string input = (lineedit -> text()).toStdString();
	if(input.length()>0){
		try{
			CBF_DEBUG("creating remote server object")
			std::cout << "connecting to " << input << std::endl;
#ifndef GUI_TEST_MODE
			XCF::RemoteServer _remoteServer = XCF::RemoteServer::create(input.c_str());
#else
//			XCF::RemoteServerPtr _remoteServer;
#endif
			Xcf_enter_remote_values_tab *new_tab = 
				new Xcf_enter_remote_values_tab(window, /*_remoteServer,*/  input);

			window -> addTab(new_tab, input.c_str());
			tabs -> push_back(new_tab);
		} catch(...){
			label -> setText("connecting failed, try it again.");
			std::cout << "connecting to '" << input << "' failed"<< std::endl;
		}
		
	}
}

void Test_xcf_reference_client_gui::quit(){
 	std::vector<Xcf_enter_remote_values_tab*>::iterator it;

	for (it = (*tabs).begin() ; it < (*tabs).end(); it++ ){
		(*it) -> quit();
	}
	
	app -> quit();
}

Xcf_enter_remote_values_tab::Xcf_enter_remote_values_tab(QWidget *parent, /*XCF::RemoteServerPtr _remoteServer,*/ std::string input):
	QWidget(parent)
{
	spinboxes = new std::vector<QDoubleSpinBox*>;

#ifndef GUI_TEST_MODE
	std::string dim_string;
	_remoteServer->callMethod("get_dimension", "", dim_string);
	CBF_DEBUG("dimension_xml: " << dim_string)

	std::istringstream vv_stream(dim_string);

	std::auto_ptr<CBFSchema::Vector> dim_v = CBFSchema::Vector_(vv_stream, xml_schema::flags::dont_validate);
	CBF::FloatVector dim_vv = CBF::create_vector(*dim_v);
	CBF_DEBUG("dim_vv: " << dim_vv)

	dim = dim_vv[0];
#else
	dim = input.size();
#endif
	layout = new QVBoxLayout(this);

	QString connectedto = "Connected to: ";
	connectedto.append(input.c_str());
	QLabel *label = new QLabel(connectedto);
	layout -> addWidget(label);
	
	addDecimalsOption();
	addStepSizeOption();
	addMinMaxoption();

	QWidget *inputWin = new QWidget();
	QGridLayout *inputWinLayout = new QGridLayout(inputWin);

	spinboxes -> reserve(dim);
	unsigned int i;
	for (i=0; i<dim; i++){
		std::ostringstream valno;
		valno << "Value Nr. " << i+1;
		inputWinLayout -> addWidget(new QLabel(valno.str().c_str(), inputWin), i, 0, 1, 1);

		QDoubleSpinBox *spinbox = new QDoubleSpinBox(inputWin);
		spinbox -> setDecimals(SPINBOX_DECIMALS);
		spinbox -> setSingleStep(SPINBOX_STEP);
		spinbox -> setMinimum(SPINBOX_MIN);
		spinbox -> setMaximum(SPINBOX_MAX);
		(*spinboxes)[i] = spinbox;
		inputWinLayout -> addWidget(spinbox, i, 2, 1, 2);
	}	

	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea -> setWidget(inputWin);
	layout -> addWidget(scrollArea);
	
	QPushButton *send = new QPushButton("Send");
	QObject::connect(send, SIGNAL(clicked()), this, SLOT(send()));	
	layout -> addWidget(send);

	QPushButton *disconnect = new QPushButton("Disconnect");
	QObject::connect(disconnect, SIGNAL(clicked()), this, SLOT(quit()));
	layout -> addWidget(disconnect);
}

/**
  * Creates and adds an option to change the count of decimals in the Spinboxes.
  */
void Xcf_enter_remote_values_tab::addDecimalsOption(){	
	decimals = new QWidget(this);
	QHBoxLayout *decimalsLayout = new QHBoxLayout(decimals);
	QPushButton *decimalsButton = new QPushButton("Set decimals to:", decimals);
	decimalsLayout -> addWidget(decimalsButton);	

	decimalSpinBox = new QSpinBox(decimals);
	decimalSpinBox -> setValue(SPINBOX_DECIMALS);
	
	decimalsLayout -> addWidget(decimalSpinBox);

	QObject::connect(decimalsButton, SIGNAL(clicked()), this, SLOT(setDecimals()));

	decimals -> setLayout(decimalsLayout);
	layout -> addWidget(decimals);
}

void Xcf_enter_remote_values_tab::addStepSizeOption(){
	stepSize = new QWidget(this);
	QHBoxLayout *stepSizeLayout = new QHBoxLayout(stepSize);
	QPushButton *stepSizeButton = new QPushButton("Set stepsize to:", stepSize);
	stepSizeLayout -> addWidget(stepSizeButton);	

	std::ostringstream tmp;
	tmp << SPINBOX_STEP;

	stepSizeLineEdit = new QLineEdit(tmp.str().c_str());
	stepSizeLayout -> addWidget(stepSizeLineEdit);

	QObject::connect(stepSizeButton, SIGNAL(clicked()), this, SLOT(setStepSize()));

	stepSize -> setLayout(stepSizeLayout);
	layout -> addWidget(stepSize);
}

void Xcf_enter_remote_values_tab::addMinMaxoption(){
	minMax = new QWidget(this);
	QGridLayout *minMaxLayout = new QGridLayout(minMax);

	QPushButton *maxButton = new QPushButton("Set maxinmum value to:", minMax);
	minMaxLayout -> addWidget(maxButton, 0, 0, 0);
	QPushButton *minButton = new QPushButton("Set minimum value to:", minMax);
	minMaxLayout -> addWidget(minButton, 1, 0, 0);

	std::ostringstream min, max;
	max << SPINBOX_MAX;
	maxLineEdit = new QLineEdit(max.str().c_str());
	minMaxLayout -> addWidget(maxLineEdit, 0, 1, 0);

	min << SPINBOX_MIN;
	minLineEdit = new QLineEdit(min.str().c_str());
	minMaxLayout -> addWidget(minLineEdit, 1, 1, 0);

	QObject::connect(minButton, SIGNAL(clicked()), this, SLOT(setMinValue()));
	QObject::connect(maxButton, SIGNAL(clicked()), this, SLOT(setMaxValue()));

	minMax -> setLayout(minMaxLayout);
	layout -> addWidget(minMax);
}


void Xcf_enter_remote_values_tab::send(){
	std::cout << "sending:";
	std::stringstream vector_string;
	vector_string << "[" << dim <<"](";
	for (unsigned int i = 0; i < dim; i++) {
		vector_string << ((*spinboxes)[i] -> value());
		if (i != dim - 1) vector_string << ",";
	}
	vector_string << ")";
	std::cout << vector_string.str() << std::endl;
	
#ifndef GUI_TEST_MODE
	CBF_DEBUG("creating vector doc")
	CBFSchema::BoostVector v(vector_string.str());

	std::ostringstream s;
	//s << v;
	CBFSchema::Vector_ (s, v);

	CBF_DEBUG("document: " << s.str())

	std::string out;

	CBF_DEBUG("calling remote method")
	_remoteServer->callMethod("set_reference", s.str(), out);
#endif
}

void Xcf_enter_remote_values_tab::changeSendMode(){
	if(alwaysSend -> isChecked()){
		for (unsigned int i = 0; i < dim; i++) {
			QObject::connect((*spinboxes)[i], 
			SIGNAL(valueChanged(double)), this, SLOT(send()));
		}
	} else {
		for (unsigned int i = 0; i < dim; i++) {
			QObject::disconnect((*spinboxes)[i], 
			SIGNAL(valueChanged(double)), this, SLOT(send()));
		}
	}
}

void Xcf_enter_remote_values_tab::quit(){	
#ifndef GUI_TEST_MODE
	_remoteServer -> destroy();
#endif
	this -> deleteLater();
}

void Xcf_enter_remote_values_tab::setDecimals(){
	int value = decimalSpinBox -> value();
	for (unsigned int i = 0; i < dim; i++) {
		QDoubleSpinBox* spinbox = (*spinboxes)[i];
		spinbox -> setDecimals(value);
	}
}

void Xcf_enter_remote_values_tab::setStepSize(){
	bool ok = false;
	double value = (stepSizeLineEdit -> text().toDouble(&ok));
	if(ok){
		for (unsigned int i = 0; i < dim; i++) {
			QDoubleSpinBox* spinbox = (*spinboxes)[i];
			spinbox -> setSingleStep(value);
		}
	} else{
		stepSizeLineEdit -> setText("Error: Double value needed.");
	}
}

void Xcf_enter_remote_values_tab::setMinValue(){
	bool ok = false;
	double value = (minLineEdit -> text().toDouble(&ok));
	bool isLess = (value < (*spinboxes)[0] -> maximum());
	if(ok){
		if(isLess){
			for (unsigned int i = 0; i < dim; i++) {
				QDoubleSpinBox* spinbox = (*spinboxes)[i];
				spinbox -> setMinimum(value);
			}
		} else{
			minLineEdit -> setText("Min has to be less then Max");
		}
	} else{
		minLineEdit -> setText("Error: Double value needed.");
	}
}

void Xcf_enter_remote_values_tab::setMaxValue(){
	bool ok = false;
	double value = (maxLineEdit -> text().toDouble(&ok));
	bool isMore = (value > (*spinboxes)[0] -> minimum());
	if(ok){
		if(isMore){
			for (unsigned int i = 0; i < dim; i++) {
				QDoubleSpinBox* spinbox = (*spinboxes)[i];
				spinbox -> setMaximum(value);
			}
		} else{
			maxLineEdit -> setText("Max has to be greater then Min");
		}
	} else{
		maxLineEdit -> setText("Error: Double value needed.");
	}
}



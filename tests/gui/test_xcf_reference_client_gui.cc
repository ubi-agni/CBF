#include <tests/gui/test_xcf_reference_client_gui.h>
#include <iostream>
#include <cbf/debug_macros.h>
#include <cbf/schemas.hxx>
#include <cbf/types.h>
#include <cbf/utilities.h>

int main(int argc, char *argv[]){
  new Test_xcf_reference_client_gui(argc, argv);
  return 0;
}

Test_xcf_reference_client_gui::Test_xcf_reference_client_gui(int argc, char *argv[]){
	spinboxes = new std::vector<QDoubleSpinBox*>;
	connected = 0;

	app = new QApplication(argc, argv);

	window = new QWidget;
	layout = new QGridLayout();
	label = new QLabel("Insert the server address here:", window);
	lineedit = new QLineEdit(window);
	okaybutton = new QPushButton("Connect", window);
	exitbutton = new QPushButton("Quit", window);

	layout -> addWidget(label, 0, 0, 1, 3);
	layout -> addWidget(lineedit, 1, 0, 1, 2);
	layout -> addWidget(okaybutton, 1, 2, 1, 1);
	layout -> addWidget(exitbutton, 2, 0, 1, 3);

	window -> setLayout(layout);
	window -> show();

	QObject::connect(exitbutton, SIGNAL(clicked()), app, SLOT(quit()));
	QObject::connect(okaybutton, SIGNAL(clicked()), this, SLOT(okay()));

	app -> exec();
}

void Test_xcf_reference_client_gui::okay(){
	if(connected){
		disconnect();
	} else{
		connect();
	}
}

void Test_xcf_reference_client_gui::connect(){
	input = (lineedit -> text()).toStdString();
	if(input.length()>0){
		try{	
			/*	
			std::cout << "connect" << std::endl;
			label -> setText("connecting...");
			CBF_DEBUG("creating remote server object")
			std::cout << "connecting to " << input << std::endl;
			_remoteServer = XCF::RemoteServer::create(input.c_str());

			std::string dim_string;
			_remoteServer->callMethod("get_dimension", "", dim_string);
			CBF_DEBUG("dimension_xml: " << dim_string)

			std::istringstream vv_stream(dim_string);

			std::auto_ptr<VectorType> dim_v = Vector(vv_stream, 				xml_schema::flags::dont_validate);
			CBF::FloatVector dim_vv = CBF::create_vector(*dim_v);
			CBF_DEBUG("dim_vv: " << dim_vv)

			dim = dim_vv[0];
			*/
			dim = 12;
			connected = 1;
			enter_input_mode();
		} catch(...){
			label -> setText("connecting failed, try it again.");
			std::cout << "connecting to '" << input << "' failed"<< std::endl;
		}
		
	}
}

void Test_xcf_reference_client_gui::disconnect(){	
	label -> setText("Insert the server address here:");
	okaybutton -> setText("Connect");	
	layout -> removeWidget(input_window);
	delete input_window;
	connected = 0;
}

void Test_xcf_reference_client_gui::enter_input_mode(){
	QString connectedto = "Connected to: ";
	connectedto.append(input.c_str());
	label -> setText(connectedto);
	okaybutton -> setText("Disconnect");

	input_window = new QWidget(window);
	QGridLayout *iwin_layout = new QGridLayout(input_window);
	
	QLabel *label2 = new QLabel("Please enter values:", input_window);

	iwin_layout -> addWidget(label2, 0 , 0, 1, 3);

	spinboxes -> reserve(dim);
	unsigned int i;
	for (i=0; i<dim; i++){
		std::ostringstream valno;
		valno << "Value Nr. " << i+1;
		iwin_layout -> addWidget(new QLabel(valno.str().c_str(), input_window), i+1, 0, 1, 1);

		QDoubleSpinBox *spinbox = new QDoubleSpinBox(input_window);
		spinbox -> setDecimals(SPINBOX_DECIMALS);
		spinbox -> setSingleStep(SPINBOX_STEP);
		(*spinboxes)[i] = spinbox;
		iwin_layout -> addWidget(spinbox, i+1, 2, 1, 2);

	}

	sendbutton = new QPushButton("Send", input_window);
	QObject::connect(sendbutton, SIGNAL(clicked()), this, SLOT(send()));

	iwin_layout -> addWidget(sendbutton, i+2, 0, 1, 4);
	input_window -> setLayout(iwin_layout);
	layout -> addWidget(input_window, 3, 0, 1, 3);
}

void Test_xcf_reference_client_gui::send(){
	std::cout << "SEND:" << std::endl;
	std::stringstream vector_string;
	vector_string << "[" << dim <<"](";
	for (unsigned int i = 0; i < dim; i++) {
		vector_string << ((*spinboxes)[i] -> value());
		if (i != dim - 1) vector_string << ",";
	}
	vector_string << ")";
	std::cout << vector_string.str() << std::endl;
	/*
	CBF_DEBUG("creating vector doc")
	BoostVectorType v(vector_string.str());

	std::ostringstream s;
	//s << v;
	Vector (s, v);

	CBF_DEBUG("document: " << s.str())

	std::string out;

	CBF_DEBUG("calling remote method")
	_remoteServer->callMethod("set_reference", s.str(), out);
	*/
}

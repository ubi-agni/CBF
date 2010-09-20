#include <tests/gui/test_xcf_reference_client_gui.h>
#include <iostream>

Test_xcf_reference_client_gui::Test_xcf_reference_client_gui(int argc, char *argv[]){
  app = new QApplication(argc, argv);

  window = new QWidget;
  label = new QLabel("Insert the server address here:");
  lineedit = new QLineEdit;
  okaybutton = new QPushButton("Connect");
  exitbutton = new QPushButton("Quit");

  layout = new QGridLayout();
  layout -> addWidget(label, 0, 0, 1, 3);
  layout -> addWidget(lineedit, 1, 0, 1, 2);
  layout -> addWidget(okaybutton, 1, 2, 1, 1);
  layout -> addWidget(exitbutton, 2, 0, 1, 3);

  window -> setLayout(layout);
  window -> show();

  QObject::connect(exitbutton, SIGNAL(clicked()), app, SLOT(quit()));
  QObject::connect(okaybutton, SIGNAL(clicked()), this, SLOT(connect()));

  app -> exec();
}

void Test_xcf_reference_client_gui::connect(){
  std::cout << "connect" << std::endl;
  CBF_DEBUG("creating remote server object")
  const char* address = (lineedit -> text()).toStdString().c_str();
  std::cout << "connecting to " << address << std::endl;
  _remoteServer = XCF::RemoteServer::create(address);
}


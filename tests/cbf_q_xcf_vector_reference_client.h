#ifndef CBF_Q_XCF_VECTOR_REFERENCE_CLIENT_HH
#define CBF_Q_XCF_VECTOR_REFERENCE_CLIENT_HH

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtCore/QObject>


#include <xcf/RemoteServer.hpp>

class Test_xcf_reference_client_gui : public QObject{
	Q_OBJECT
	
	private:
	static const int SPINBOX_DECIMALS = 9;
	static const double SPINBOX_STEP = 0.0001;	
	static const double SPINBOX_MIN = -1000;
	static const double SPINBOX_MAX = +1000;
	
	QApplication *app;
	QWidget *window;
	QWidget *connectWindow;
	QWidget *inputWindow;
	QLabel *label;
	QLineEdit *lineedit;
	QPushButton *okaybutton;
	QPushButton *exitbutton;
	QPushButton *sendbutton;
	QVBoxLayout *layout;
	QCheckBox *alwaysSend;
	unsigned int connected;

	XCF::RemoteServerPtr _remoteServer;
	std::string input; //input-string for Servername
	unsigned int dim;
	std::vector<QDoubleSpinBox*> *spinboxes;
	//QDoubleSpinBox *spinboxes;
  
	void enter_input_mode();
	void connect();
	void disconnect();

	public:
	Test_xcf_reference_client_gui(int argc, char *argv[]);

	public slots:
	void okay();
	void send();
	void changeSendMode();
};

#endif

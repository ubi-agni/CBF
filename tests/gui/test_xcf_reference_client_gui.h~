#ifndef TEST_XCF_REFERENCE_CLIENT_GUI_HH
#define TEST_XCF_REFERENCE_CLIENT_GUI_HH

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtCore/QObject>

#include <cbf/debug_macros.h>
#include <xcf/RemoteServer.hpp>

class Test_xcf_reference_client_gui : public QObject{
  Q_OBJECT
  
  private:
  QApplication *app;
  QWidget *window;
  QLabel *label;
  QLineEdit *lineedit;
  QPushButton *okaybutton;
  QPushButton *exitbutton;
  QGridLayout *layout;

  XCF::RemoteServerPtr _remoteServer;
  
  
  public:
  Test_xcf_reference_client_gui(int argc, char *argv[]);

  public slots:
  void connect();
};

#endif

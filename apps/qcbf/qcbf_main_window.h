/* -*- mode: c-non-suck; -*- */

#ifndef QCBF_MAIN_WINDOW_HH
#define QCBF_MAIN_WINDOW_HH

#include <QtGui/QMainWindow>

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		MainWindow(QWidget *parent, QString title) :
			QMainWindow(parent)
		{
			setWindowTitle(title);
		}
};

#endif


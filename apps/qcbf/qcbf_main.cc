/* -*- mode: c-non-suck; -*- */

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

#include <cbf/plugin_pool.h>
#include <cbf/resource.h>

#include <qcbf_main_window.h>

int main(int argc, char **argv) {
	QApplication qapp(argc, argv);

	MainWindow win(0, "qCBF");
	win.show();

	CBF::ResourcePtr p = CBF::PluginPool<CBF::Resource>::get_instance()->create(0);

	return(qapp.exec());
}


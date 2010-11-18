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


#ifndef CBF_Q_XCF_VECTOR_REFERENCE_CLIENT_HH
#define CBF_Q_XCF_VECTOR_REFERENCE_CLIENT_HH

#include <xcf/RemoteServer.hpp>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>

#include <cbf/types.h>

/** 
	@file cbf_q_xcf_vector_reference_client.h
	@brief The class declarations for the cbf_q_xcf_reference_client application
	@author Viktor Richter
 */



namespace cbf_q_xcf_vector_reference_client {
/** 
	@brief A QWidget that can be used to set and load the reference of a controller over the network.
	It gets the dimension of the controller through the (already initialized) XCF::RemoteServerPtr
	and shows the appropriate amound of spinboxes to set new values and send them.
*/
class Connection_manager : public QWidget{
	Q_OBJECT

	private:
	/** 
		@brief The minimum count of decimals in a spinbox is:
	*/	
	static const int SPINBOX_DECIMALS_MIN = 0;
	/** 
		@brief The maximum count of decimals in a spinbox is:
	*/	
	static const int SPINBOX_DECIMALS_MAX = 99;
	/** 
		@brief The initial count of decimals in a spinbox is:
	*/	
	static const int SPINBOX_DECIMALS = 5;
	/** 
		@brief The initial size of a single step in a spinbox is:
	*/	
	static const double SPINBOX_STEP = 0.00005;	
	/** 
		@brief The initial minimum value in a spinbox is:
	*/	
	static const double SPINBOX_MIN = -1000;
	/** 
		@brief The initial maximum value in a spinbox is:
	*/	
	static const double SPINBOX_MAX = +1000;
	/** 
		@brief Is the Options-widget visible at first?
	*/	
	static const bool SHOW_OPTIONS = false;
	/** 
		@brief Is the always send option activatet at first?
	*/	
	static const bool ALWAYS_SEND = false;
	/** 
		@brief The shortest pause time before a new task position is loaded from the server.
	*/	
	static const int RELOAD_PAUSE_TIME_MIN = 50;
	/** 
		@brief The longest pause time before a new task position is loaded from the server.
	*/	
	static const int RELOAD_PAUSE_TIME_MAX = 60000;
	/** 
		@brief The stepsize for the spinbox that holds the current reload pause time.
	*/	
	static const int RELOAD_PAUSE_TIME_STEP = 100;
	/** 
		@brief The standart pause time before a new task position is loaded from the server.
	*/	
	static const int RELOAD_PAUSE_TIME_FIRST = 1000;
	/** 
		@brief Is the always load option activated at first?
	*/	
	static const bool ALWAYS_LOAD = false;


	/** 
		@brief The RemoteServerPtr, which is used for the server communication.
	*/	
	XCF::RemoteServerPtr _remoteServer;

	/** 
		@brief The dimension of the remote controller.
	*/	
	unsigned int dim;
	
	/** 
		@brief A vector to save the pointers to the Spinboxes (used for changing the behaviour and
		getting the current the values of the spinboxes).
	*/	
	std::vector<QDoubleSpinBox*> *spinboxes;

	/** 
		@brief The timer that emits the signal to load the curent task position.
	*/	
	QTimer *timer;

	/** 
		@brief A vector to save the pointers to the Labels that show the current task position of
		the remote controller.
	*/	
	std::vector<QLabel*> *currentValueLabels;

	/** 
		@brief The widget that holds the spinboxes.
	*/
	QWidget *inputWin;

	/** 
		@brief The widget that holds the options for the spinboxes and servercommunication.
	*/
	QTabWidget *optionsTabWidget;

	/** 
		@brief The checkbox which shows/hides the options-widget.
	*/	
	QCheckBox *optionsCheckBox;

	/** 
		@brief The checkbox which activates the always-send function.
	*/	
	QCheckBox *alwaysSendCheckBox;	

	/** 
		@brief The checkbox which activates the always-load function.
	*/	
	QCheckBox *alwaysLoadCheckBox;

	/** 
		@brief The spinbox that holds the pause-time of the always load option.
	*/	
	QSpinBox *reloadPauseSpinbox;

	/** 
		@brief The LineEdit for the new count of decimals for the spinboxes.
	*/	
	QLineEdit *decimalsLineEdit;

	/** 
		@brief The LineEdit for the new single step size for the spinboxes.
	*/	
	QLineEdit *stepSizeLineEdit;

	/** 
		@brief The LineEdit for the new minimum value for the spinboxes.
	*/	
	QLineEdit *minLineEdit;

	/** 
		@brief The LineEdit for the new maximum value for the spinboxes.
	*/	
	QLineEdit *maxLineEdit;

	/** 
		@brief The ComboBox, for the 'set spinboxes to' function.
	*/
	QComboBox *comboSetSpinboxes;


	/** 
		@brief Creates a QTabWidget for the pointer optionsTabWidget and fills it with options for the
		servercommunication and spinbox-behaviour. The widget needs to be added to the layout afterwards.
	*/
	void makeOptionsWidget();

	public:
	/** 
		@brief The constructor of the Connection_manager gets the dimension from the
		server and opens the adequate number ob QDoubleSpinboxes.

		@param parent The parent-widget of the manager.
		@param _remoteServer The XCF::RemoteServerPtr, which is used to communicate with the server.
		@param input The entered servername. it is only used to show the user the name of the server.
	*/
	Connection_manager(QWidget *parent, XCF::RemoteServerPtr _remoteServer, std::string input);

	public slots:

	/** 
		@brief Disconnects from the remoteServer and closes the tab.
	*/
	void disconnect();

	/** 
		@brief Creates a n-dimensional vector from the values of the spinboxes and sends it to the remoteServer.
	*/
	void send();

	/** 
		@brief Changes the sending mode from 'send on sendbutton' to 'always Send' and back.
	*/
	void changeSendMode();

	/** 
		@brief Gets the current position from the server. Returns a pointer to a CBF:FloatVector.
		ATTENTION: dont forget to free the space with 'delete'.
	*/
	CBF::FloatVector* loadCurrentPositionVector();

	/** 
		@brief Gets the current position from the server and writes it into the labels next to the spinboxes.
	*/
	void loadRemoteValues();

	/** 
		@brief Changes the load mode from 'load on loadbutton' to 'always load' and back.
	*/
	void changeLoadMode();

	/** 
		@brief Shows and hides the QWidget, that contains the options for the spinboxes.
		
		@param time The pause-time after which the timer sends its signal.
	*/
	void changeLoadPauseTime(int time);

	/** 
		@brief Sets the spinboxes to the specified values.
	*/
	void setSpinboxesTo();

	/** 
		@brief Shows and hides the QWidget, that contains the options for the spinboxes.
	*/
	void showOptionsWidget();

	/** 
		@brief Sets the count of decimals of the spinboxes to the entered.
	*/
	void setDecimals();

	/** 
		@brief Sets the single-step-size of the spinboxes to the entered.
	*/
	void setStepSize();

	/** 
		@brief Sets the minimum value of the spinboxes to the entered.
	*/
	void setMinValue();

	/** 
		@brief Sets the maximum value of the spinboxes to the entered.
	*/
	void setMaxValue();
};


/** 
	@brief A QObject which can build up multiple connections to XCFServers. This can be used to set the reference
	of a controller over the network. It only creates the RemoteServerPtr and lets the Connection_manager 
	communicate with the server.
 */
class Connection_dispatcher : public QObject{
	Q_OBJECT
	
	/** 
		@brief The minimum width of the window.
	*/
	static const int PROG_MIN_WIDTH = 250;
	/** 
		@brief The minimum heith of the window.
	*/
	static const int PROG_MIN_HEIGTH = 450;
	
	private:
	/** 
		@brief The main window of the application
	*/
	QTabWidget *window;
	/** 
		@brief The line where the server name can be specified.
	*/
	QLineEdit *lineedit;
	
	/** 
		@brief A list of all open Connection_manager tabs.
	*/
	std::vector<Connection_manager*> *tabs;

	public:
	/** 
	@brief Opens a window in which connections to XCF::RemoteServers can be build up. 
	The connections are opend in tabs.

		@param argc The command line argument count.
		@param argv The command line arguments.
	 */
	Connection_dispatcher(int argc, char *argv[]);

	public slots:
	/** 
	@brief Tries to build up a connection to the server specified in the lineedit.
	*/
	void connect();
};
} //namespace

#endif

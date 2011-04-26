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


    Copyright 20011 Viktor Richter
*/

#include <cbf/debug_macros.h>
#include <cbf/exceptions.h>

#include <Memory/Interface.hpp>

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QErrorMessage>
#include <QtGui/QCheckBox>

/**
	@brief A struct that can be used to create XCFMemoryRunController[...] documents
	and send them to an active_memory.
*/
class XcfMemoryRunControllerOperator : public QWidget {
	Q_OBJECT

	private:

		/**
			@brief The name of the XCFMemoryRunController. This name will be used in
			all XCFMemoryRunController documents.
		*/
		std::string m_RunControllerName;

		/**
			@brief A pointer is used to send documents to an active_memory.
		*/
		memory::interface::MemoryInterface::pointer m_MemoryInterface;

		/**
			@brief The Path we look for documents in.
		*/
		std::string m_DirPath;

		/**
			@brief We will save the filenames and attachment-id's of the send documents.
		*/
		std::map<std::string, std::string> m_AttachmentNames;

	public:

		/**
			@brief Sets the run_controller_name and connects to an active_memory
			throws a runtime_error when connection can't be established.

			@param active_memory_name The name/location of the actice_memory
			@param run_controller_name The name of the XCFMemoryRunController to control.
		*/
		XcfMemoryRunControllerOperator(std::string active_memory_name, std::string run_controller_name)
		:
		m_RunControllerName(run_controller_name)
		{
			try {
				m_MemoryInterface = (memory::interface::MemoryInterface::getInstance(active_memory_name));	
				m_DirPath = "";
				m_AttachmentNames;
			} catch (...) {
				CBF_THROW_RUNTIME_ERROR("can't connect to memory: " << active_memory_name);
			}
		}

	public slots:

		/**
			@brief Opens a file-dialog and sends all chosen files as attachments.
		*/
		void send_descriptions();

		/**
			@brief Triggers the run_controller to load a list of files into an ObjectNamespace.
		*/
		void create_namespace();

		/**
			@brief Opens a Dialog in which a time can be chosen.
			The time is send in an XCFMemoryRunControllerOptions.
		*/
		void set_time();

		/**
			@brief Opens a Dialog in which a step count can be chosen.
			The step count is send in an XCFMemoryRunControllerOptions.
		*/
		void set_steps();

		/**
			@brief Opens a Dialog in which a controller names can be defined.
			The controller name is send in an XCFMemoryRunControllerExecute.
		*/
		void execute();

		/**
			@brief Sends an XCFMemoryRunControllerStop.
		*/
		void stop();

};


/**
	@brief A struct used by the XcfMemoryRunControllerOperator. It shows a
	list of files to choose from and returns the corresponding attachment-id's.
*/
class XcfMemoryRunControllerDocumentDialog : public QDialog {
	Q_OBJECT

	private:

	/**
		@brief The checkboxes are listed here.
	*/
	std::vector<QCheckBox*> m_QCheckBoxes;

	/**
		@brief A helper function that initializes the widget.
		@param attachment_map The attachment-id's mapped to the corresponding filenames.
	*/
	void init(std::map<std::string, std::string> attachment_map);



	public:
	/**
		@brief Initializes this QDialog end fills it with widets.
		@param attachment_map The attachment-id's mapped to the corresponding filenames.
		@param parent The parent-widget of this.
	*/
	XcfMemoryRunControllerDocumentDialog(std::map<std::string, std::string> attachment_map, QWidget* parent = 0)
	:
	QDialog(parent),
	m_QCheckBoxes()
	{
		init(attachment_map);
	}

	public slots:

	/**
		@brief executes this dialog (modal)
	*/
	std::vector<std::string> exec();

};

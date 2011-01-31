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

class XcfMemoryRunControllerOperator : public QWidget {
	Q_OBJECT

	private:

		std::string m_RunControllerName;
		memory::interface::MemoryInterface::pointer m_MemoryInterface;		

	public:

		XcfMemoryRunControllerOperator(std::string active_memory_name, std::string run_controller_name)
		:
		m_RunControllerName(run_controller_name)
		{
			try {
				m_MemoryInterface = (memory::interface::MemoryInterface::getInstance(active_memory_name));		
			} catch (...) {
				CBF_THROW_RUNTIME_ERROR("can't connect to memory: " << active_memory_name);
			}
		}

	public slots:
		void add_control_basis();
		void set_time();
		void set_steps();
		void execute();
		void stop();
		void load_controllers();
};

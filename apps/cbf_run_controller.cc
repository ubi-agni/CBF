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


    Copyright 2011 Viktor Richter
*/

#include <cbf_run_controller.h>

namespace CBF {

	CBFRunController::CBFRunController(
				unsigned int sleep_time,
				unsigned int steps,
				unsigned int verbosity_level
				#ifdef CBF_HAVE_QT
					,bool qt_support
				#endif
				)
		:
		m_SleepTime(sleep_time),
		m_Steps(steps),
		m_VerbosityLevel(verbosity_level),
		m_ControllerRunning(false),
		m_ControlBasisSet(false)
		#ifdef CBF_HAVE_QT
			,m_QtSupport(qt_support)
		#endif
	{
	}

	void CBFRunController::start_controller(std::string controller_name){
		if(!checkControlBasisSet()){
			std::cout << "controll_basis is not set" << std::endl;
			return;
		}

		if (m_ControlBasis -> controllers().find(controller_name) 
			== m_ControlBasis -> controllers().end()){
			std::cout << "controller name not found in control basis" << std::endl;
			return;			
		}

		if(checkControllerRuns(true)){
			std::cout << "a controller is already running." << std::endl;
			return;
		}

		while ((stepCount() == 0) && (m_ControlBasis->controllers()[controller_name]->step() == false)) {

			if (!checkControllerRuns()) //stops execution
				{break; }

			if (verbosityLevel())
				{ std::cout << "step" << std::endl; }

			usleep(sleepTime() * 1000);
			#ifdef CBF_HAVE_QT
				if (qtSupport()) QApplication::processEvents();
			#endif
		}
		if  (stepCount() > 0){
			while(stepCount() > 0){
				if (verbosityLevel())
					{ std::cout << "step" << std::endl; }

				if (!checkControllerRuns()) //stops execution
					{break; }

				m_ControlBasis -> controllers()[controller_name]->step();
				usleep(sleepTime() * 1000);
				#ifdef CBF_HAVE_QT
					if (qtSupport()) QApplication::processEvents();
				#endif
				decStepCount();
			}
		}
		stop_controller();
	}

	void CBFRunController::stop_controller(){
		checkControllerRuns(false);
	}

	void CBFRunController::setSleepTime(unsigned int time){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_SleepTimeMonitor);
		m_SleepTime = time;
	}

	unsigned int CBFRunController::sleepTime(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_SleepTimeMonitor);
		return m_SleepTime;
	}

	void CBFRunController::setStepCount(unsigned int steps){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_StepsMonitor);
		m_Steps = steps;
		
	}

	unsigned int CBFRunController::stepCount(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_StepsMonitor);
		return m_Steps;
	}

	void CBFRunController::decStepCount(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_StepsMonitor);
		m_Steps--;
	}

	void CBFRunController::setVerbosityLevel(unsigned int verbosity_level){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_VerbosityLevelMonitor);
		m_VerbosityLevel = verbosity_level;
	}

	unsigned int CBFRunController::verbosityLevel(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_VerbosityLevelMonitor);
		return m_VerbosityLevel;
	}

	bool CBFRunController::checkControllerRuns(bool running){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		if(m_ControllerRunning){
			m_ControllerRunning = running;
			return true;
		} else {
			m_ControllerRunning = running;
			return false;		
		}
	}

	bool CBFRunController::checkControllerRuns(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		return m_ControllerRunning;
	}

	bool CBFRunController::checkControlBasisSet(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		return m_ControlBasisSet;
	}

	void CBFRunController::setControlBasis(CBF::ControlBasisPtr control_basis){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		if(!m_ControllerRunning){
			m_ControlBasis = control_basis;
			m_ControlBasisSet = true;
		} else {
			std::cout << "could not set control_basis because controller is running" << std::endl;
		}
	}

#ifdef CBF_HAVE_QT
	void CBFRunController::setQTSupport(bool qt_support){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_QtSupportMonitor);
		m_QtSupport = qt_support;
	}

	bool CBFRunController::qtSupport(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_QtSupportMonitor);
		return m_QtSupport;
	}
#endif

} //namespace

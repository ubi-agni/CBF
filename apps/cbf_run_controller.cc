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
	//everything already initialized
	}

	void CBFRunController::start_controller(std::string controller_name)
		throw(ControlBasisNotSetException, ControllerNotFoundExcepption, ControllerRunningException)
	{

		//does not start if the controll_basis is not set.
		if(!checkControlBasisSet()){
			throw ControlBasisNotSetException();
		}

		//does nothing when the controller is not in the control_basis
		if (m_ControlBasis -> controllers().find(controller_name) 
			== m_ControlBasis -> controllers().end()){
			throw ControllerNotFoundExcepption();
		}

		//does nothing if the controller is already running.
		if(checkControllerRuns(true)){
			throw ControllerRunningException();
		}

		// if the stepcount is 0 we are stepp()ing till convergence
		// setting stepCount != 0 in execution will make us leave the while-clause
		while ((stepCount() == 0) && (m_ControlBasis->controllers()[controller_name]->step() == false)) {

			if (!checkControllerRuns()) //stops execution
				{return; }

			if (verbosityLevel())
				{ std::cout << "step" << std::endl; }

			usleep(sleepTime() * 1000);
			#ifdef CBF_HAVE_QT
				if (qtSupport()) QApplication::processEvents();
			#endif
		}
		// does stepCount() steps
		while(stepCount() > 0){
			if (verbosityLevel())
				{ std::cout << "step" << std::endl; }

			if (!checkControllerRuns()) //stops execution
				{return; }

			m_ControlBasis -> controllers()[controller_name]->step();
			usleep(sleepTime() * 1000);
			#ifdef CBF_HAVE_QT
				if (qtSupport()) QApplication::processEvents();
			#endif
			decStepCount();
		}
		//setting m_ControllerRunning to false.
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
		bool was_running = m_ControllerRunning;		
		m_ControllerRunning = running;
		return was_running;
	}

	bool CBFRunController::checkControllerRuns(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		return m_ControllerRunning;
	}

	bool CBFRunController::checkControlBasisSet(){
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		return m_ControlBasisSet;
	}

	void CBFRunController::setControlBasis(CBF::ControlBasisPtr control_basis) 
		throw(ControllerRunningException)
	{
		//controller should not be working when we change the control_basis
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		if(!m_ControllerRunning){
			m_ControlBasis = control_basis;
			m_ControlBasisSet = true;
		} else {
			throw ControllerRunningException();
		}
	}

	const CBF::ControlBasisPtr CBFRunController::getControlBasis() 
		 throw(ControlBasisNotSetException)
	{
		//controller should not be working while we copy the control_basis
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ControllerRunningMonitor);
		if (m_ControlBasisSet) {
			return CBF::ControlBasisPtr(new CBF::ControlBasis(*m_ControlBasis));
		} else {
			throw ControlBasisNotSetException();
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

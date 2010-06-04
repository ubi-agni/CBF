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

#ifndef CBF_ROBOTINTERFACE_RESOURCE_HH
#define CBF_ROBOTINTERFACE_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/resource.h>
#include <cbf/plugin_decl_macros.h>

#include <boost/thread.hpp>

#include <stdexcept>
#include <algorithm>

#include <Memory/SynchronizedQueue.hpp>
#include <riRobotInterface.h>
#include <riEventHandler.h>
#include <riRobotEvent.h>
#include <riPoseEvent.h>


CBF_PLUGIN_PREAMBLE(RobotInterfaceResource)

namespace CBF {

struct RobotInterfaceResource : public Resource, public robotinterface::EventHandler {
	CBF_PLUGIN_DECL_METHODS(RobotInterfaceResource)

	RobotInterfaceResource(
		const std::string &memory_uri, 
		const std::string &robot_name,
		unsigned int dimension
	)
	{
		init(memory_uri, robot_name, dimension);
	}

	void init(
		const std::string &memory_uri, 
		const std::string &robot_name,
		unsigned int dimension
	) {
		m_RobotName = robot_name;
		m_RobotInterface.connect(memory_uri),

		m_Result = FloatVector(dimension);
		m_LastPose = FloatVector(dimension);

		m_RobotInterface.subscribe(
			robotinterface::RobotInterface::INSERT | robotinterface::RobotInterface::REPLACE,
			"/EVENT[@name='WorldModelUpdate']/ROBOT[@id='" + robot_name + "']",
			this
		);

		m_RobotCommandSet = robotinterface::RobotCommandSet(robot_name, true, true);
		m_RobotCommandSet.defaults(robot_name) << robotinterface::cmd::moveMode("stp", "joint");
	}


	virtual void handle(const robotinterface::RobotEvent *e) {
		boost::recursive_mutex::scoped_lock lock(m_ResultMutex);

		const robotinterface::PoseEvent *pe = 
			dynamic_cast<const robotinterface::PoseEvent*>(e);

		if (pe == 0) 
			throw std::runtime_error("Unexpected Event Type: Expected PoseEvent");

		const std::vector<float> &tmp = pe->getPose(robotinterface::PoseEvent::POSTURE);

		if (tmp.size() != m_LastPose.size())
			throw std::runtime_error("Dimension mismatch");

		std::copy(tmp.begin(), tmp.end(), m_LastPose.begin());
	}


	virtual void update() {
		boost::recursive_mutex::scoped_lock lock(m_ResultMutex);

		m_Result = m_LastPose;		
	}

	virtual void add(const FloatVector &arg) {
		assert(std::cout << "slobodan was here" << std::endl);

		m_RobotCommandSet 
			<< robotinterface::cmd::clear()
			<< robotinterface::cmd::posture(arg.begin(), arg.end(), "rad");

		m_RobotInterface.send(m_RobotCommandSet);
	}

	virtual const FloatVector &get() {
		return m_Result;
	}

	protected:
		robotinterface::RobotInterface m_RobotInterface;	
		robotinterface::RobotCommandSet m_RobotCommandSet;

		std::string m_RobotName;

		FloatVector m_Result;
		FloatVector m_LastPose;

		boost::recursive_mutex m_ResultMutex;		
};

typedef boost::shared_ptr<RobotInterfaceResource> RobotInterfaceResourcePtr;

} // namespace

#endif

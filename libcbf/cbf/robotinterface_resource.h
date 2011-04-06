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
#include <cbf/debug_macros.h>
#include <cbf/resource.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/thread.hpp>

#include <stdexcept>
#include <algorithm>
#include <string>

#include <Memory/SynchronizedQueue.hpp>

#include <riRobotInterface.h>
#include <riEventHandler.h>
#include <riRobotEvent.h>
#include <riRobotState.h>
#include <riPoseEvent.h>


namespace CBFSchema { class RobotInterfaceResource; }

namespace CBF {

	struct RobotInterfaceResource : public Resource, public robotinterface::EventHandler {
		RobotInterfaceResource (const CBFSchema::RobotInterfaceResource &xml_instance, ObjectNamespacePtr object_namespace);
	
		RobotInterfaceResource(
			const std::string &send_memory_uri = "", 
			const std::string &recv_memory_uri = "", 
			const std::string &robot_name = "",
			unsigned int dimension = 0
		)
		{
	    CBF_DEBUG("pre");
			init(
				send_memory_uri, 
				recv_memory_uri, 
				robot_name, 
				dimension
			);
	    CBF_DEBUG("post");
		}
	
		void init(
			const std::string &send_memory_uri, 
			const std::string &recv_memory_uri, 
			const std::string &robot_name,
			unsigned int dimension
		) {
			CBF_DEBUG("init start: " << robot_name);
			boost::recursive_mutex::scoped_lock lock(m_ResultMutex);
	
			m_RobotName = robot_name;
			m_RobotInterface.connect(send_memory_uri, recv_memory_uri),
	
			m_Result = FloatVector(dimension);
			m_LastPose = FloatVector(dimension);
	
			m_RobotCommandSet = robotinterface::RobotCommandSet(robot_name, true, true);
			m_RobotCommandSet.defaults(robot_name) << robotinterface::cmd::moveMode("stp", "joint");
	
			CBF_DEBUG("querying position of robot: " << robot_name);
			robotinterface::RobotState state = m_RobotInterface.query(
				robot_name, robotinterface::RobotState::QUERY_POSTURE
			);

			/*FIXME:
			std::copy(
				state.getPosture().begin(), 
				state.getPosture().end(),
				m_Result.()
			);
			*/

			for (int i = 0; i < state.getPosture().size(); ++i){
				m_Result(i) = state.getPosture().at(i);
			}
	
			m_LastPose = m_Result;
	
			m_RobotInterface.subscribe(
				robotinterface::RobotInterface::INSERT | robotinterface::RobotInterface::REPLACE,
				"/EVENT[@name='WorldModelUpdate']/ROBOT[@id='" + robot_name + "']",
				this
			);
			CBF_DEBUG("init end");
		}
	
	
		virtual void handle(const robotinterface::RobotEvent *e) {
			//CBF_DEBUG("handle")
	
			// CBF_DEBUG("xml: " << e->getXML())
			boost::recursive_mutex::scoped_lock lock(m_ResultMutex);
	
			const robotinterface::PoseEvent *pe = 
				dynamic_cast<const robotinterface::PoseEvent*>(e);
	
			if (pe == 0) 
				throw std::runtime_error("Unexpected Event Type: Expected PoseEvent");
	
			const std::vector<float> &tmp = pe->getPose(robotinterface::PoseEvent::POSTURE, m_RobotName);
	
			// std::cout << "dimesion: " << tmp.size() << std::endl;
	
			if (tmp.size() != m_LastPose.size())
				throw std::runtime_error("Dimension mismatch");
	
			//FIXME: std::copy(tmp.begin(), tmp.end(), m_LastPose.begin());

			for (int i = 0; i < tmp.size(); ++i){
				m_LastPose(i) = tmp.at(i);
			}
	
			//CBF_DEBUG("LAST POSE: " << m_LastPose << std::endl)
		}
	
	
		virtual void update() {
	    CBF_DEBUG("update");
			boost::recursive_mutex::scoped_lock lock(m_ResultMutex);
	
			m_Result = m_LastPose;		
		}
	
		virtual void add(const FloatVector &arg) {
			FloatVector eigentmp = m_Result + arg;
			std::vector<Float> tmp;
			for (int i = 0; i < m_Result.size(); ++i){
				tmp.push_back(eigentmp(i));
			}

			CBF_DEBUG("add tmp: " << tmp);
			m_RobotCommandSet
							<< robotinterface::cmd::clear()
							<< robotinterface::cmd::posture(tmp.begin(), tmp.end(), "rad");
	
			m_RobotInterface.send(m_RobotCommandSet);
		}
	
		virtual const FloatVector &get() 
		{ 
			boost::recursive_mutex::scoped_lock lock(m_ResultMutex);
			return m_Result; 
		}
	
		virtual void set(const FloatVector &arg) { 
			boost::recursive_mutex::scoped_lock lock(m_ResultMutex);
			CBF_THROW_RUNTIME_ERROR("this doesn't make sense")
		}
	
		virtual unsigned int dim() {
			boost::recursive_mutex::scoped_lock lock(m_ResultMutex);
			return m_Result.size();
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

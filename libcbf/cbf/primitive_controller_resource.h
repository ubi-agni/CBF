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

#ifndef CBF_PRIMITIVE_CONTROLLER_RESOURCE_HH
#define CBF_PRIMITIVE_CONTROLLER_RESOURCE_HH

#include <cbf/config.h>

#include <cbf/primitive_controller.h>
#include <cbf/resource.h>

namespace CBFSchema { class PrimitiveControllerResource; }

namespace CBF {
	
	/**
		@brief This type represents a primitive controller as resource. 
	
		The current value of the resource is the controller's 
		sensor-transformed own resource value. 
		An incremental resource update step is realized as setting
		the controller's reference to the current sensor transformed resource
		value plus the incremental resource update step.
	
		The controller is then run until convergence.
	
		This mechanism allows the stacking of controllers in such a way,
		that the one's task space is the other's resource space.
	
		A useful application of this scheme is e.g. stacking a visual servoing
		controller "on top" of a robotic arm end effector position 
		controller. In this application the robotic arm controller is acting as
		resource to the visual servoing controller
	
	*/
	struct PrimitiveControllerResource : public Resource {
		PrimitiveControllerResource (const CBFSchema::PrimitiveControllerResource &xml_instance);
	
		PrimitiveControllerPtr m_PrimitiveController;
	
		FloatVector m_Result;
	
		PrimitiveControllerResource(PrimitiveControllerPtr controller = PrimitiveControllerPtr()) :
			m_PrimitiveController(controller),
		{
		}
	
		public:
			virtual void read() {
				m_PrimitiveController->sensor_transform()->read();
				m_PrimitiveController->sensor_transform()->exec(m_Result);
			}
	
			virtual const FloatVector &get() {
				return m_Result;
			}
	
			virtual void add(const FloatVector &arg) {
				set(m_Result + arg);
			}
	
			virtual void set(const FloatVector &arg) {
				//! Setup reference..
				m_PrimitiveController->set_reference(arg);
	
				//! And run controller until convergence..
				do {
					m_PrimitiveController->step();
				} while (!m_PrimitiveController->finished());
			}
	
			virtual unsigned int dim() {
				return m_PrimitiveController->sensor_transform()->get_task_dim();
			}
	};
	
	class PrimitiveControllerResource;
	typedef boost::shared_ptr<PrimitiveControllerResource> PrimitiveControllerResourcePtr;
} // namespace

#endif

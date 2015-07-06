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
		@brief This type represents a primitive controller as a resource for a higher level controller.
	
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

  enum ENUM_INTEGRATION_BASE {INTEGRATION_BASE_REFERENCE=0, INTEGRATION_BASE_SENSOR};

	struct PrimitiveControllerResource : public Resource {
#ifdef CBF_HAVE_XSD
		PrimitiveControllerResource (
			const CBFSchema::PrimitiveControllerResource &xml_instance,
			ObjectNamespacePtr object_namespace);
#endif
		PrimitiveControllerPtr m_PrimitiveController;
	
    PrimitiveControllerResource(PrimitiveControllerPtr controller, ENUM_INTEGRATION_BASE integrationBase=INTEGRATION_BASE_SENSOR) :
      m_PrimitiveController(controller),
      m_IntegrationBase(integrationBase)
		{
		}
	
		public:
      virtual void read()
      {
        m_PrimitiveController->resource()->read();
        m_PrimitiveController->sensor_transform()->update(m_PrimitiveController->resource()->get_position());

        m_ResourceValue = m_PrimitiveController->sensor_transform()->result();
        m_ResourceValueVelocity = m_PrimitiveController->sensor_transform()->get_task_velocity();
			}

      virtual void write(const FloatVector &vel, const Float timestep)
      {
	
        if(m_IntegrationBase == INTEGRATION_BASE_REFERENCE) {
          //! Setup reference..
          std::vector<FloatVector>& refs = m_PrimitiveController->reference()->get();

          integrate_Euler(refs[0], vel, timestep);
          m_PrimitiveController->reference()->set_reference(refs[0]);
        }
        else {
          FloatVector lRef = m_ResourceValue;
          integrate_Euler(lRef, vel, timestep);
          m_PrimitiveController->reference()->set_reference(lRef);
        }

        m_PrimitiveController->step();

        ////! And run controller until convergence..
        //do {
        //	m_PrimitiveController->step();
        //} while (!m_PrimitiveController->finished());
			}
	
    private:
      unsigned int m_IntegrationBase;

	};
	
	class PrimitiveControllerResource;
	typedef boost::shared_ptr<PrimitiveControllerResource> PrimitiveControllerResourcePtr;
} // namespace

#endif

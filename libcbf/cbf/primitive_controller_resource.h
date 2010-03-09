#ifndef CBF_PRIMITIVE_CONTROLLER_RESOURCE_HH
#define CBF_PRIMITIVE_CONTROLLER_RESOURCE_HH

#include <cbf/config.h>

#include <cbf/primitive_controller.h>
#include <cbf/resource.h>
#include <cbf/plugin_decl_macros.h>

CBF_PLUGIN_PREAMBLE(PrimitiveControllerResource)

namespace CBF {
	
	/**
		@brief This type represents a primitive controller as resource. 
	
		The current
		value of the resource is the controller's sensor transformed resource
		value. An incremental resource update step is realized as setting
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
		CBF_PLUGIN_DECL_METHODS(PrimitiveControllerResource)
	
		PrimitiveControllerPtr m_PrimitiveController;
	
		CBFFloatVector m_Result;
	
		std::vector<CBFFloatVector > m_References;
	
		PrimitiveControllerResource(PrimitiveControllerPtr controller = PrimitiveControllerPtr()) :
			m_PrimitiveController(controller),
			m_References(1)
		{
	
		}
	
		public:
			virtual void read() {
				m_PrimitiveController->sensor_transform()->read();
				m_PrimitiveController->sensor_transform()->exec(m_Result);
			}
	
			virtual const CBFFloatVector &get() {
				return m_Result;
			}
	
			virtual void add(const CBFFloatVector &arg) {
				set(m_Result + arg);
			}
	
			virtual void set(const CBFFloatVector &arg) {
				//! Setup reference..
				m_References[0] = arg;
				m_PrimitiveController->set_references(m_References);
	
				// CBF_DEBUG("Running controller 'til convergence...")
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

#ifndef CBF_COMPOSITE_PRIMITIVE_CONTROLLER_HH
#define CBF_COMPOSITE_PRIMITIVE_CONTROLLER_HH

#include <cbf/controller.h>
#include <cbf/primitive_controller.h>
#include <cbf/plugin_decl_macros.h>

#include <boost/shared_ptr.hpp>

CBF_PLUGIN_PREAMBLE(CompositePrimitiveController);

namespace CBF {

/**
	@brief This class allows the "vertical" stacking of controllers in the 
	sense that the task space is the combined task spaces of the 
	individual components..
*/
struct CompositePrimitiveController : public Controller {
	CBF_PLUGIN_DECL_METHODS(CompositePrimitiveController);

	CompositePrimitiveController(const std::vector<PrimitiveControllerPtr> controllers) {
		set_controllers(controllers);
		m_Controllers = controllers;
	}

	CompositePrimitiveController(PrimitiveControllerPtr c1, PrimitiveControllerPtr c2) {
		std::vector<PrimitiveControllerPtr> controllers;
		set_controllers(controllers);
	}

	const std::vector<PrimitiveControllerPtr> &controllers() {
		return m_Controllers;
	}

	void set_controllers(const std::vector<PrimitiveControllerPtr> controllers) {
		m_Controllers = controllers;
	}

	virtual void do_update(int cycle) {
		for (unsigned int i = 0; i < m_Controllers.size(); ++i)
			m_Controllers[i]->do_update(cycle);
	}

	virtual void do_action(int cycle) {
		for (unsigned int i = 0; i < m_Controllers.size(); ++i)
			m_Controllers[i]->do_action(cycle);
	}

	protected:
		std::vector<PrimitiveControllerPtr> m_Controllers;
};

typedef boost::shared_ptr<CompositePrimitiveController> CompositePrimitiveControllerPtr;

} // namespace

#endif

#include <cbf/control_basis.h>
#include <cbf/plugin_pool.h>

namespace CBF {

#ifdef CBF_HAVE_PLUGIN_SUPPORT 
	ControlBasis::ControlBasis(const ControlBasisType &xml_instance) {
		for (
			ControlBasisType::Controller_const_iterator it = xml_instance.Controller.begin();
			it != xml_instance.Controller.end();
			++it)
		{
			ControllerPtr c = PluginPool<Controller>::get_instance()->create_from_xml(*it);
			m_Controllers.push_back(c);
		}
	}
#endif

} // namespace
#include <cbf/control_basis.h>
#include <cbf/plugin_pool.h>

#ifdef CBF_HAVE_PLUGIN_SUPPORT
	#include <schemas.hxx>
#endif

namespace CBF {

#ifdef CBF_HAVE_PLUGIN_SUPPORT 
	ControlBasis::ControlBasis(const ControlBasisType &xml_instance) {
		for (
			ControlBasisType::Controller_const_iterator it = 
				xml_instance.Controller().begin();
			it != xml_instance.Controller().end();
			++it)
		{
			ControllerPtr c = 
				PluginPool<Controller>::get_instance()->create_from_xml(*it);

			m_Controllers[c->name()] = c;
		}
	}
#endif

} // namespace

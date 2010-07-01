#include <cbf/control_basis.h>
#include <cbf/plugin_pool.h>
#include <cbf/debug_macros.h>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

namespace CBF {

#ifdef CBF_HAVE_XSD
	ControlBasis::ControlBasis(const ControlBasisType &xml_instance) {
		CBF_DEBUG("Building Control Basis")
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

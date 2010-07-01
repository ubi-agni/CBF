#ifndef CBF_CONTROL_BASIS_HH
#define CBF_CONTROL_BASIS_HH

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <cbf/controller.h>
#include <cbf/plugin_decl_macros.h>

CBF_PLUGIN_PREAMBLE(ControlBasis)

namespace CBF {

struct ControlBasis {
	CBF_PLUGIN_DECL_METHODS(ControlBasis)

	ControlBasis () { }
	
	typedef std::map<std::string, ControllerPtr> ControllerMap;
	ControllerMap m_Controllers;

	ControllerMap &controllers() {
		return m_Controllers;
	}
};

typedef boost::shared_ptr<ControlBasis> ControlBasisPtr;

} // namespace

#endif

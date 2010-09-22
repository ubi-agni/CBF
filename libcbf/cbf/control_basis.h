#ifndef CBF_CONTROL_BASIS_HH
#define CBF_CONTROL_BASIS_HH

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <cbf/controller.h>

namespace CBFSchema { class ControlBasis; }

namespace CBF {

struct ControlBasis {
	ControlBasis () { }

	ControlBasis(const CBFSchema::ControlBasis &xml_instance);
	
	typedef std::map<std::string, ControllerPtr> ControllerMap;
	ControllerMap m_Controllers;

	ControllerMap &controllers() {
		return m_Controllers;
	}
};

typedef boost::shared_ptr<ControlBasis> ControlBasisPtr;

} // namespace

#endif

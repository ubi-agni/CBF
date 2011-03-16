#ifndef CBF_CONTROL_BASIS_HH
#define CBF_CONTROL_BASIS_HH

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <cbf/controller.h>
#include <cbf/namespace.h>
#include <cbf/object.h>

namespace CBFSchema { class ControlBasis; }

namespace CBF {

struct ControlBasis : public Object {
	ControlBasis () : Object("ControlBasis") { }

	ControlBasis(const CBFSchema::ControlBasis &xml_instance, ObjectNamespacePtr object_namespace);
	
	typedef std::map<std::string, ControllerPtr> ControllerMap;
	ControllerMap m_Controllers;

	ControllerMap &controllers() {
		return m_Controllers;
	}
};

typedef boost::shared_ptr<ControlBasis> ControlBasisPtr;

} // namespace

#endif

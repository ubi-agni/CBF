#include <cbf/control_basis.h>
#include <cbf/debug_macros.h>
#include <cbf/exceptions.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	ControlBasis::ControlBasis(const CBFSchema::ControlBasis &xml_instance) {
		CBF_DEBUG("Building Control Basis")
		for (
			CBFSchema::ControlBasis::Controller_const_iterator it = 
				xml_instance.Controller().begin();
			it != xml_instance.Controller().end();
			++it)
		{
			ControllerPtr c = 
				XMLBaseFactory<Controller, CBFSchema::Controller>::instance()->create(*it);

			if (m_Controllers.find(c->name()) != m_Controllers.end()) {
				CBF_THROW_RUNTIME_ERROR("Name: " << c->name() << " already in basis");
			}
				
			m_Controllers[c->name()] = c;
		}
	}

	static XMLDerivedFactory<
		ControlBasis, 
		CBFSchema::ControlBasis, 
		Object, 
		CBFSchema::Object> 
	x1;

#endif

} // namespace

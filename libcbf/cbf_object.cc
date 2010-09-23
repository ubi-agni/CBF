#include <cbf/cbf_object.h>
#include <cbf/xml_factories.h>

namespace CBF {

#ifdef CBF_HAVE_XSD

	template<> XMLBaseFactory<CBFObject, CBFSchema::CBFObject>* 
		XMLBaseFactory<CBFObject, CBFSchema::CBFObject>::m_Instance = 0;

#endif

} // namespace


#include <cbf/object.h>
#include <cbf/xml_factories.h>

namespace CBF {

#ifdef CBF_HAVE_XSD

	template<> XMLBaseFactory<Object, CBFSchema::Object>* 
		XMLBaseFactory<Object, CBFSchema::Object>::m_Instance = 0;

#endif

} // namespace


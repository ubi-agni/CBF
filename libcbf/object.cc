#include <cbf/object.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	Object::Object(const CBFSchema::Object &xml_instance) {
		if (xml_instance.Name().present()) {
			CBF_DEBUG("name.present() == true");
			m_Name = *(xml_instance.Name());
		} else {
			m_Name = CBF_UNMANGLE(xml_instance);
		}
	}
#endif

} // namespace


#include <cbf/object.h>
#include <cbf/xml_object_factory.h>
#include <cbf/namespace.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	Object::Object(const CBFSchema::Object &xml_instance, boost::shared_ptr<ObjectNamespace> object_namespace) {
		if (xml_instance.Name().present()) {
			CBF_DEBUG("name.present() == true");
			m_Name = *(xml_instance.Name());
		} else {
			m_Name = CBF_UNMANGLE(xml_instance);
		}
	}
#endif

} // namespace


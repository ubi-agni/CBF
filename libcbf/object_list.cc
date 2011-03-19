#include <cbf/object_list.h>
#include <cbf/xml_factory.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
		ObjectList::ObjectList(const CBFSchema::ObjectList &xml_instance, ObjectNamespacePtr object_namespace) :
			Object(xml_instance, object_namespace)
		{
			for(
				CBFSchema::ObjectList::Object1_const_iterator it 
					= xml_instance.Object1().begin(); 
				it != xml_instance.Object1().end(); 
				++it
			) {
				
			}
		}

	#endif
} // namespace
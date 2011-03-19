#ifndef CBF_OBJECT_LIST_HH
#define CBF_OBJECT_LIST_HH

#include <cbf/object.h>
#include <cbf/namespace.h>

#include <vector>


namespace CBFSchema { class ObjectList; }

namespace CBF {
	struct ObjectList : public Object {
		ObjectList() : Object("ObjectList") {

		}

		ObjectList(const CBFSchema::ObjectList &xml_instance, ObjectNamespacePtr object_namespace);

		std::vector<ObjectPtr> m_Objects;
	};

	typedef boost::shared_ptr<ObjectList> ObjectListPtr;
} // namespace

#endif


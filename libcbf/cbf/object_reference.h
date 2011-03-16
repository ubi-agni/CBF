#ifndef CBF_OBJECT_REFERENCE_HH
#define CBF_OBJECT_REFERENCE_HH

#include <cbf/types.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <string>

namespace CBFSchema {
	class ObjectReference;
}

namespace CBF {

struct ObjectReference : public Object {
	std::string m_ReferredObjectName;

	ObjectReference(const std::string &referred_object_name) :
		Object(std::string("ObjectReference ") + referred_object_name) {
		init(referred_object_name);
	}

	ObjectReference(const CBFSchema::ObjectReference &xml_instance, ObjectNamespacePtr object_namespace);

	void init(const std::string &referred_object_name) {
		m_ReferredObjectName = referred_object_name;
	}
};

} // namespace

#endif
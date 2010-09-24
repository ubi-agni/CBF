#ifndef CBF_OBJECT_HH
#define CBF_OBJECT_HH

#include <cbf/config.h>

#include <boost/shared_ptr.hpp>
#include <string>

namespace CBFSchema { class Object; }

namespace CBF {

	struct Object {
		std::string m_Name;
	
		Object(const CBFSchema::Object &xml_instance);
		Object(const std::string name = "Object") : m_Name(name) { }
	};
	
	typedef boost::shared_ptr<Object> ObjectPtr;
} // namespace

#endif

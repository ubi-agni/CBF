#ifndef CBF_OBJECT_HH
#define CBF_OBJECT_HH

#include <cbf/config.h>
//#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>
#include <string>

namespace CBFSchema { class Object; }

namespace CBF {
	struct ObjectNamespace;

	/**
		@brief The common base class for all CBF types. 

		This type is introduced solely to ease some of the XML stuff
	*/
	struct Object {
		std::string m_Name;
	
		Object(const CBFSchema::Object &xml_instance, boost::shared_ptr<ObjectNamespace> object_namespace);

		Object(const std::string name) : m_Name(name) { }
		//Object(const std::string name = "Object") : m_Name(name) { }

		virtual ~Object() { }
		const std::string &name() { return m_Name; }
	};
	
	typedef boost::shared_ptr<Object> ObjectPtr;
} // namespace

#endif

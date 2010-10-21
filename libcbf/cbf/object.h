#ifndef CBF_OBJECT_HH
#define CBF_OBJECT_HH

#include <cbf/config.h>

#include <boost/shared_ptr.hpp>
#include <string>

namespace CBFSchema { class Object; }

namespace CBF {

	/**
		@brief The common base class for all CBF types. 

		This type is introduced solely to ease some of the XML stuff
	*/
	struct Object {
		std::string m_Name;
	
		Object(const CBFSchema::Object &xml_instance);
		Object(const std::string name = "Object") : m_Name(name) { }

		virtual ~Object() { }
	};
	
	typedef boost::shared_ptr<Object> ObjectPtr;
} // namespace

#endif

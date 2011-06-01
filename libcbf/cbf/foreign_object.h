#ifndef CBF_FOREIGN_OBJECT_WRAPPER_HH
#define CBF_FOREIGN_OBJECT_WRAPPER_HH

#include <cbf/xml_factory.h>
#include <cbf/object.h>
#include <cbf/exceptions.h>
#include <cbf/debug_macros.h>

namespace CBF {
	/** 
		@brief A class used to wrap instances of classes that are not derived from CBF::Object (e.g. KDL::Tree)
	*/
	template<class T> 
	struct ForeignObject : public Object {
		boost::shared_ptr<T> m_Object;

		ForeignObject(boost::shared_ptr<T> object) :
			Object("ForeignObject"),
			m_Object(object)
		{
			if (m_Object.get() == 0)
				CBF_THROW_RUNTIME_ERROR("trying to register empty object");
		}

		#ifdef CBF_HAVE_XSD
		ForeignObject(
			const CBFSchema::Object &xml_instance, 
			ObjectNamespacePtr object_namespace	
		) : 
			Object("ForeignObject")
		{
			if (xml_instance.Name().present()) {
				CBF_DEBUG("creating ForeignObject from xml element with name: " << *xml_instance.Name());
				m_Name = *xml_instance.Name();
			}
			m_Object = XMLFactory<T>::instance()->create(xml_instance, object_namespace);
		}
		#endif
	};
} // namespace


#endif

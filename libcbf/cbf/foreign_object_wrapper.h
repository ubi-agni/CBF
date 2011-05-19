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
	struct ForeignObjectWrapper : public Object {
		boost::shared_ptr<T> m_WrappedObject;

		ForeignObjectWrapper(boost::shared_ptr<T> object) :
			Object("ForeignObjectWrapper"),
			m_WrappedObject(object)
		{
			if (m_WrappedObject.get() == 0)
				CBF_THROW_RUNTIME_ERROR("trying to register empty object");
		}


		ForeignObjectWrapper(
			const CBFSchema::Object &xml_instance, 
			boost::shared_ptr<ObjectNamespace> object_namespace	
		) : 
			Object("ForeignObjectWrapper")
		{
			if (xml_instance.Name().present()) {
				CBF_DEBUG("creating ForeignObjectWrapper from xml element with name: " << *xml_instance.Name());
				m_Name = *xml_instance.Name();
			}
			// m_Object = XMLFactory<KDL::Tree>::instance()->create(xml_instance);
			m_WrappedObject = XMLFactory<T>::instance()->create(xml_instance, object_namespace);
		}


	};
} // namespace


#endif

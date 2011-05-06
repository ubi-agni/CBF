#ifndef CBF_FOREIGN_OBJECT_WRAPPER_HH
#define CBF_FOREIGN_OBJECT_WRAPPER_HH

#include <cbf/xml_factory.h>
#include <cbf/object.h>
#include <cbf/exceptions.h>

namespace CBF {
	template<class T> 
	struct ForeignObjectWrapper : public Object {
		boost::shared_ptr<T> m_Object;

		ForeignObjectWrapper(boost::shared_ptr<T> object) :
			Object("ForeignObjectWrapper"),
			m_Object(object)
		{
			if (m_Object.get() == 0)
				CBF_THROW_RUNTIME_ERROR("trying to register empty object");
		}

#if 0
		ForeignObjectWrapper(
			const CBFSchema::Object &xml_instance, 
			boost::shared_ptr<ObjectNamespace> object_namespace	
		) : 
			m_Object(boost::shared_ptr<T>(new T("ewfew"))) 
		{
			// m_Object = XMLFactory<KDL::Tree>::instance()->create(xml_instance);
		}
#endif

	};
} // namespace


#endif

#ifndef CBF_NAMESPACE_HH
#define CBF_NAMESPACE_HH

#include <cbf/object.h>
#include <cbf/exceptions.h>
#include <cbf/debug_macros.h>
#include <cbf/xml_factory.h>

#include <boost/shared_ptr.hpp>
#include <map>

namespace CBF {
	struct ObjectNamespace {
		typedef std::map<std::string, ObjectPtr> map;

		map m_Map;

		/**
			This method throws an error when throw_if_fails is true and either
			the key is not found or the object is not castable to the desired 
			target type T

			if throw_if_fails is false, then this method never throws an exception\
			but instead returns a boost::shared_ptr<T> with .get() == 0
		*/
		template<class T> boost::shared_ptr<T> get(const std::string &key, bool throw_if_fails = true) {
			CBF_DEBUG("trying to get object with name \"" << key << "\" and of type \"" << CBF_UNMANGLE(T) << "\"");
			if(m_Map.find(key) == m_Map.end()) {
				CBF_THROW_RUNTIME_ERROR("no object in map with key: " << key);
			}

			try {
				ObjectPtr p = m_Map[key];
				if (p.get() == 0) CBF_THROW_RUNTIME_ERROR("object in map is 0 pointer");

				boost::shared_ptr<T> ret = boost::dynamic_pointer_cast<T>(p);

				if (ret.get() == 0) {
					CBF_THROW_RUNTIME_ERROR("cast failed");
				} 

				return ret;
			} catch (...) {
				if (throw_if_fails) 
					throw;
				else 
					return boost::shared_ptr<T>();
			}
		}

		void register_object(const std::string &key, ObjectPtr object) {
			CBF_DEBUG("registering object with name: " << key);
			m_Map[key] = object;

			CBF_DEBUG("namespace contents");

			for (map::iterator it = m_Map.begin(); it != m_Map.end(); ++it) {
				CBF_DEBUG("\"" << (*it).first << "\" -> " << (*it).second);
			}
		}
	};

	typedef boost::shared_ptr<ObjectNamespace> ObjectNamespacePtr;

} // namespace

#endif

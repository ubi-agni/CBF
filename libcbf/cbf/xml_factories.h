#ifndef CBF_XML_FACTORIES_HH
#define CBF_XML_FACTORIES_HH

#include <cbf/config.h>
#include <cbf/exceptions.h>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	template <class TBase>
	struct XMLDerivedFactoryBase {
		virtual boost::shared_ptr<TBase> create(const ::xml_schema::type &xml_instance) = 0;
	};

	template <class TBase, class TBaseType>
	struct XMLBaseFactory {
		protected:
			static XMLBaseFactory *m_Instance;
			XMLBaseFactory() { 
					CBF_DEBUG("instance (possibly mangled type name follows): " << CBF_UNMANGLE(typeid(this).name()))
			}

		public:
			std::vector<XMLDerivedFactoryBase<TBase>* > m_DerivedFactories;

			static XMLBaseFactory *instance() { 
				if (m_Instance) 
					{ return m_Instance; }
				return (m_Instance = new XMLBaseFactory<TBase, TBaseType>); 
			}

			virtual boost::shared_ptr<TBase> create(const ::xml_schema::type &xml_instance) {
				for (unsigned int i = 0, max = m_DerivedFactories.size(); i != max; ++i) {
					boost::shared_ptr<TBase> p = m_DerivedFactories[i]->create(xml_instance);
					if (p.get()) return p;
				}
				CBF_THROW_RUNTIME_ERROR(
					"No creator found for this " 
					<< CBF_UNMANGLE(typeid(this).name()) << xml_instance
				)

				return boost::shared_ptr<TBase>();
			}
	};

	template <class T, class TType, class TBase, class TBaseType>
	struct XMLDerivedFactory : XMLDerivedFactoryBase<TBase> {
			XMLDerivedFactory() { 
				CBF_DEBUG("register (possibly mangled type name follows): " << CBF_UNMANGLE(typeid(this).name()))
				XMLBaseFactory<TBase, TBaseType>::instance()->m_DerivedFactories.push_back(this); 
			}

		public:
			virtual boost::shared_ptr<TBase> create(const ::xml_schema::type &xml_instance) {
				CBF_DEBUG("am i the one? possibly mangled name follows: " << CBF_UNMANGLE(typeid(this).name()))
				const TType* r = dynamic_cast<const TType*>(&xml_instance);
				if (r) {
					CBF_DEBUG("yes, i am the one")
					return boost::shared_ptr<TBase>(new T(*r));
				}
				CBF_DEBUG("no i am not the one")
				return boost::shared_ptr<TBase>();
			}
	};


#endif

} // namespace

#endif


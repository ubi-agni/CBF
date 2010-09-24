#ifndef CBF_XML_FACTORIES_HH
#define CBF_XML_FACTORIES_HH

#include <cbf/config.h>
#include <cbf/exceptions.h>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>
#include <cbf/object.h>

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

namespace CBF {

	#ifdef CBF_HAVE_XSD
		/**
			This type is only available when XSD support is enabled via the CBF_HAVE_XSD macro define
		*/
		template <class TBase>
		struct XMLDerivedFactoryBase {
			virtual boost::shared_ptr<TBase> create(const ::xml_schema::type &xml_instance) = 0;
		};
	
		/**
			This type is only available when XSD support is enabled via the CBF_HAVE_XSD macro define
		*/
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
	
		/**
			This type is only available when XSD support is enabled via the CBF_HAVE_XSD macro define
		*/
		template <class T, class TType, class TBase, class TBaseType>
		struct XMLDerivedFactory : XMLDerivedFactoryBase<TBase> {
				XMLDerivedFactory() { 
					CBF_DEBUG("registering (possibly mangled type name follows): " << CBF_UNMANGLE(typeid(this).name()))
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

		template <class T>
		struct XMLCreatorBase {
			virtual boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance) = 0;
			virtual ~XMLCreatorBase() { }
		};
	
	
		template <class T>
		struct XMLFactory {
			std::map<std::string, XMLCreatorBase<T>* > m_Creators;
	
			static XMLFactory *instance() {
				if (m_Instance == 0)
					return (m_Instance = new XMLFactory());
			}
	
			virtual boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance) {
				CBF_DEBUG(
					"creating a " << 
					CBF_UNMANGLE(typeid(T).name()) << 
					" from a " << 
					CBF_UNMANGLE(typeid(xml_instance).name())
				)

				return m_Creators[typeid(xml_instance).name()]->create(xml_instance);
			}

			virtual ~XMLFactory() { }

			protected:
				XMLFactory() { }
				static XMLFactory *m_Instance;
		};

		template<class T, class TSchemaType, class C>
		struct XMLCreator : public XMLCreatorBase <T>{
			C m_Creator;
	
			XMLCreator(C c) : m_Creator(c) { 
				// register here..
				XMLFactory<T>::instance()->m_Creators[typeid(TSchemaType).name()] = this;
				CBF_DEBUG(
					"registering type: " << 
					CBF_UNMANGLE(typeid(TSchemaType).name()) << 
					" in registry " << 
					CBF_UNMANGLE(typeid(T).name())
				)
			}

			boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance) {
				CBF_DEBUG(
					"creating a " << 
					CBF_UNMANGLE(typeid(T).name()) << 
					" from a " << 
					CBF_UNMANGLE(typeid(xml_instance).name()) <<
					" TSChemaType = " <<
					CBF_UNMANGLE(typeid(TSchemaType).name())
				)
				const TSchemaType &tmp = dynamic_cast<const TSchemaType&>(xml_instance);
				return m_Creator(tmp);
			}
		};
	
	#endif
	
} // namespace

#endif


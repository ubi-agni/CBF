#ifndef CBF_XML_OBJECT_FACTORY_HH
#define CBF_XML_OBJECT_FACTORY_HH

#include <cbf/config.h>
#include <cbf/exceptions.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif


namespace CBF {

		/**
			@brief The base type for all concrete CBF::Object factories
		*/
		struct XMLDerivedFactoryBase {
			virtual boost::shared_ptr<Object> create(const CBFSchema::Object &xml_instance, ObjectNamespacePtr object_namespace) = 0;
			virtual ~XMLDerivedFactoryBase() { }
		};

		/**
			@brief The central registry, where all types derived of CBF::Object that have
			a constructor taking a CBFSchema::Object argument in their constructor register.

			This factory is used for all CBFSchema types that can inherit from CBF::Object. 
			This is not true for things like CBFSchema::BoostVector or CBFSchema::ZeroMatrix.
			For these types the XMLFactory is used
		*/
		struct XMLObjectFactory {
			protected:
				static XMLObjectFactory *m_Instance;
				XMLObjectFactory() { 

				}
	
			public:
				virtual ~XMLObjectFactory() { }
	
				std::map<std::string, XMLDerivedFactoryBase* > m_DerivedFactories;
	
				static XMLObjectFactory *instance() { 
					if (m_Instance) 
						{ return m_Instance; }

					return (m_Instance = new XMLObjectFactory); 
				}


				/**
					@brief This function can be used to create an Object from a CBFSchema::Object or
					derived type instance. 

					If the creation fails, a std::runtime_error is thrown
				*/
				template<class T>
				boost::shared_ptr<T> create(
					const CBFSchema::Object &xml_instance, 
					ObjectNamespacePtr object_namespace
				) {
					if (m_DerivedFactories.find(std::string(typeid(xml_instance).name())) == m_DerivedFactories.end()) {
						CBF_THROW_RUNTIME_ERROR(
							"No factory found for type (possibly mangled): " << 
							CBF_UNMANGLE(xml_instance)
						);
					}

					boost::shared_ptr<T> p = 
						boost::dynamic_pointer_cast<T>(
							m_DerivedFactories[typeid(xml_instance).name()]->create(xml_instance, object_namespace)
						)
					;

					if (p.get()) return p;

					CBF_THROW_RUNTIME_ERROR(
						"No factory found for type (possibly mangled): " << 
						CBF_UNMANGLE(xml_instance)
					);
	
					return boost::shared_ptr<T>();
				}
		};
	
		/**
			This type is only available when XSD support is enabled via the CBF_HAVE_XSD macro define
		*/
		template <class T, class TType>
		struct XMLDerivedFactory : public XMLDerivedFactoryBase {
				XMLDerivedFactory() { 
					CBF_DEBUG(
						"registering (possibly mangled type name follows): " << 
						CBF_UNMANGLE(T) << 
						" with SchemaType: " << 
						CBF_UNMANGLE(TType)
					);
					XMLObjectFactory::instance()->m_DerivedFactories[std::string(typeid(TType).name())] = this; 
				}
	
			public:
				virtual boost::shared_ptr<Object> create(
					const CBFSchema::Object &xml_instance, 
					ObjectNamespacePtr object_namespace
				) {
					CBF_DEBUG("am i the one? possibly mangled name follows: " << CBF_UNMANGLE(this));
					const TType* r = dynamic_cast<const TType*>(&xml_instance);
					if (r) {
						CBF_DEBUG("yes, i am the one");
						ObjectPtr p(new T(*r, object_namespace));
						object_namespace->register_object(p->name(), p);
						return p;
					}
					CBF_DEBUG("no i am not the one");
					return boost::shared_ptr<Object>();
				}
		};

} // namespace


#endif

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
			@brief The base type for all concrete Object factories
		*/
		struct XMLDerivedFactoryBase {
			virtual boost::shared_ptr<Object> create(const CBFSchema::Object &xml_instance) = 0;
		};
	
		/**
			@brief The central registry, where all types derived of CBF::Object that have
			a constructor taking a CBFSchema::Object argument in their constructor register.
		*/
		struct XMLObjectFactory {
			protected:
				static XMLObjectFactory *m_Instance;
				XMLObjectFactory() { 
						CBF_DEBUG("instance (possibly mangled type name follows): " << CBF_UNMANGLE(typeid(this).name()))
				}
	
			public:
				std::vector<XMLDerivedFactoryBase* > m_DerivedFactories;
	
				static XMLObjectFactory *instance() { 
					if (m_Instance) 
						{ return m_Instance; }
					return (m_Instance = new XMLObjectFactory); 
				}

				template<class T>
				boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance) {
					for (unsigned int i = 0, max = m_DerivedFactories.size(); i != max; ++i) {
						boost::shared_ptr<T> p = boost::dynamic_pointer_cast<T>(m_DerivedFactories[i]->create(xml_instance));
						if (p.get()) return p;
					}
					CBF_THROW_RUNTIME_ERROR(
						"No creator found for this " 
						<< CBF_UNMANGLE(typeid(this).name()) << xml_instance
					)
	
					return boost::shared_ptr<T>();
				}
		};
	
		/**
			This type is only available when XSD support is enabled via the CBF_HAVE_XSD macro define
		*/
		template <class T, class TType>
		struct XMLDerivedFactory : XMLDerivedFactoryBase {
				XMLDerivedFactory() { 
					CBF_DEBUG("registering (possibly mangled type name follows): " << CBF_UNMANGLE(typeid(this).name()))
					XMLObjectFactory::instance()->m_DerivedFactories.push_back(this); 
				}
	
			public:
				virtual boost::shared_ptr<Object> create(const CBFSchema::Object &xml_instance) {
					CBF_DEBUG("am i the one? possibly mangled name follows: " << CBF_UNMANGLE(typeid(this).name()))
					const TType* r = dynamic_cast<const TType*>(&xml_instance);
					if (r) {
						CBF_DEBUG("yes, i am the one")
						return boost::shared_ptr<Object>(new T(*r));
					}
					CBF_DEBUG("no i am not the one")
					return boost::shared_ptr<Object>();
				}
		};


		/**
			@brief A functor that can be used to register types in the XMLFactory that do not
			provide a free function to construct them from a TSchemaType but rather
			a constructor..
		*/
		template <class T, class TSchemaType>
		struct Constructor {
			boost::shared_ptr<T> operator()(const TSchemaType &xml_instance) {
				CBF_DEBUG("creating a " << CBF_UNMANGLE(typeid(T).name()))
				const TSchemaType &t = dynamic_cast<const TSchemaType&>(xml_instance);
				return boost::shared_ptr<T>(new T(t));
			}
		};

		template <class T>
		struct XMLCreatorBase {
			virtual boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance) = 0;
			virtual ~XMLCreatorBase() { }
		};
	

		/**
			The XMLFactory is a central point for types to register ways of constructing instances
			from CBFSchema:: types..
		*/ 	
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

				if (m_Creators.find(typeid(xml_instance).name()) == m_Creators.end()) {
					CBF_THROW_RUNTIME_ERROR(
						"[" << CBF_UNMANGLE(typeid(this).name())<< "]: "  << 
						"XMLCreator for type not found. Type: " << 
						CBF_UNMANGLE(typeid(xml_instance).name()) << 
						" (Did you forget to register it?)"
					)
				}

				return m_Creators[typeid(xml_instance).name()]->create(xml_instance);
			}

			virtual ~XMLFactory() { }

			protected:
				XMLFactory() { }
				static XMLFactory *m_Instance;
		};

		/**
			This template allows registering a functor C that
			constructs a boost::shared_ptr<T> from a TSchemaType.

			THe signature of the functor's operator() has to be

			boost::shared_ptr<T>(*)(const TSchemaType&)
		*/
		template<class T, class TSchemaType, class C>
		struct XMLCreator : public XMLCreatorBase <T> {
			C m_Creator;
	
			XMLCreator(C c) : m_Creator(c) { 
				CBF_DEBUG(
					"registering type: " << 
					CBF_UNMANGLE(typeid(TSchemaType).name()) << 
					" in registry " << 
					CBF_UNMANGLE(typeid(T).name())
				)

				XMLFactory<T>::instance()->m_Creators[typeid(TSchemaType).name()] = this;
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


		/**
			This utility template can be used with types that have a constructor
			that takes a const TSchemaType& as argument.
		*/
		template<class TBase, class T, class TSchemaType>
		struct XMLConstructorCreator : public XMLCreator<
			T, TSchemaType, Constructor<T, TSchemaType> 
		> {
			XMLConstructorCreator() : 
				XMLCreator<
					T, TSchemaType, 
					Constructor<
						T, TSchemaType
					> 
				> (Constructor<T, TSchemaType>()) 
			{ 
				CBF_DEBUG(
					"adding constructor with TBase: " << 
					CBF_UNMANGLE(typeid(TBase).name()) <<  
					" and T: " << 
					CBF_UNMANGLE(typeid(T).name()) << 
					" and TSchemaType: " << 
					CBF_UNMANGLE(typeid(TSchemaType).name())
				)
			}
		};
	#endif
	
} // namespace

#endif


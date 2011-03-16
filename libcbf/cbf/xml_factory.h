	/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#ifndef CBF_XML_FACTORIES_HH
#define CBF_XML_FACTORIES_HH

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
	#ifdef CBF_HAVE_XSD
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


		template <class T>
		struct XMLCreatorBase {
			virtual boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance, ObjectNamespacePtr object_namespace) = 0;
			virtual ~XMLCreatorBase() { }
		};
	

		/**
			@brief The XMLFactory is a central point for types to register ways of constructing instances from CBFSchema:: types..

			This type of factory is used for types that do not inherit CBF::Object like e.g.
			FloatVector or FloatMatrix
		*/ 	
		template <class T>
		struct XMLFactory {
			std::map<std::string, XMLCreatorBase<T>* > m_Creators;
	
			static XMLFactory *instance() {
				if (m_Instance == 0)
					return (m_Instance = new XMLFactory());

				return m_Instance;
			}
	
			virtual boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance, ObjectNamespacePtr object_namespace) {
				CBF_DEBUG(
					"creating a " << 
					CBF_UNMANGLE(T) << 
					" from a " << 
					CBF_UNMANGLE(xml_instance)
				);

				if(xml_instance.ReferencedObjectName().present()) {
					boost::shared_ptr<ForeignObjectWrapper<T> > fptr = 
						object_namespace->get<ForeignObjectWrapper<T> >(
							*xml_instance.ReferencedObjectName()
						)
					;

					CBF_DEBUG("m_Object has type: " << CBF_UNMANGLE(fptr->m_Object));
					boost::shared_ptr<T> ptr = fptr->m_Object;

					return ptr;
				}

				if (m_Creators.find(typeid(xml_instance).name()) == m_Creators.end()) {
					CBF_THROW_RUNTIME_ERROR(
						"[" << CBF_UNMANGLE(this)<< "]: "  << 
						"XMLCreator for type not found. Type: " << 
						CBF_UNMANGLE(xml_instance) << 
						" (Did you forget to register it?)"
					);
				}

				boost::shared_ptr<T> ptr = m_Creators[typeid(xml_instance).name()]->create(xml_instance, object_namespace);

				if (xml_instance.Name().present()) {
					CBF_DEBUG("registering foreign object");
					object_namespace->register_foreign_object<T>(*xml_instance.Name(), ptr);
				}
				return ptr;
			}

			virtual ~XMLFactory() { }

			protected:
				XMLFactory() { }
				static XMLFactory *m_Instance;
		};

		/**
			@brief This template allows registering a functor C that
			constructs a boost::shared_ptr<T> from a TSchemaType.

			THe signature of the functor's operator() has to be

			boost::shared_ptr<T>(*)(const TSchemaType&)
		*/
		template<class T, class TSchemaType, class C>
		struct XMLCreator : public XMLCreatorBase <T> {
			C m_Creator;
	
			XMLCreator(C c) : m_Creator(c) { 
				CBF_DEBUG(
					"XMLCreator registering type: " << 
					CBF_UNMANGLE(TSchemaType) << 
					" in registry " << 
					CBF_UNMANGLE(T)
				);

				XMLFactory<T>::instance()->m_Creators[typeid(TSchemaType).name()] = this;
			}

			boost::shared_ptr<T> create(const CBFSchema::Object &xml_instance, ObjectNamespacePtr object_namespace) {
				CBF_DEBUG(
					"creating a " << 
					CBF_UNMANGLE(T) << 
					" from a " << 
					CBF_UNMANGLE(xml_instance) <<
					" TSChemaType = " <<
					CBF_UNMANGLE(TSchemaType)
				);
				const TSchemaType &tmp = dynamic_cast<const TSchemaType&>(xml_instance);
				boost::shared_ptr<T> ptr = m_Creator(tmp, object_namespace);

				if(xml_instance.Name().present()) {
					object_namespace->register_foreign_object<T>(*xml_instance.Name(), ptr);
				}

				return ptr;
			}
		};


		/**
			@brief A functor that can be used to register types in the XMLFactory that do not
			provide a free function to construct them from a TSchemaType but rather
			a constructor..
		*/
		template <class T, class TSchemaType>
		struct Constructor {
			Constructor() { CBF_DEBUG("Constructor"); }
			boost::shared_ptr<T> operator()(const TSchemaType &xml_instance, ObjectNamespacePtr object_namespace) {
				CBF_DEBUG("creating a " << CBF_UNMANGLE(T));
				const TSchemaType &t = dynamic_cast<const TSchemaType&>(xml_instance);
				return boost::shared_ptr<T>(new T(t, object_namespace));
			}
		};



		/**
			This utility template can be used with types that have a constructor
			that takes a const TSchemaType& as argument.
		*/
		template<class TBase, class T, class TSchemaType>
		struct XMLConstructorCreator : public XMLCreator<
			TBase, TSchemaType, Constructor<T, TSchemaType> 
		> {
			XMLConstructorCreator() : 
				XMLCreator<
					TBase, TSchemaType, 
					Constructor<
						T, TSchemaType
					> 
				> (Constructor<T, TSchemaType>()) 
			{ 
				CBF_DEBUG(
					"adding constructor with TBase: " << 
					CBF_UNMANGLE(TBase) <<  
					" and T: " << 
					CBF_UNMANGLE(T) << 
					" and TSchemaType: " << 
					CBF_UNMANGLE(TSchemaType)
				);
			}
		};
	#endif
} // namespace

#endif


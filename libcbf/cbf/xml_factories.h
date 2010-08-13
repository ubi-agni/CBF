#ifndef CBF_XML_FACTORIES_HH
#define CBF_XML_FACTORIES_HH

#include <cbf/exceptions.h>
#include <cbf/schemas.hxx>

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
			XMLBaseFactory() { }


		public:
			std::vector<XMLDerivedFactoryBase<TBase>* > m_DerivedFactories;
			/**

			*/
			static XMLBaseFactory *instance() { 
				if (m_Instance) 
					{ return m_Instance; }
				else 
					{ return (m_Instance = new XMLBaseFactory<TBase, TBaseType>); }
			}

			virtual boost::shared_ptr<TBase> create(const ::xml_schema::type &xml_instance) {
				for (unsigned int i = 0, max = m_DerivedFactories.size(); i != max; ++i) {
					boost::shared_ptr<TBase> p = m_DerivedFactories[i]->create(xml_instance);
					if (p.get()) return p;
				}
				CBF_THROW_RUNTIME_ERROR("No creator found for this " << TBase::type_name() << xml_instance)
				return boost::shared_ptr<TBase>();
			}
	};

	

	template <class T, class TType, class TBase, class TBaseType>
	struct XMLDerivedFactory : XMLDerivedFactoryBase<TBase> {
#if 0
		protected:
			static XMLDerivedFactory *m_Instance;
#endif
			XMLDerivedFactory() 
				{ XMLBaseFactory<TBase, TBaseType>::instance()->m_DerivedFactories.push_back(this); }

		public:
#if 0
		static XMLDerivedFactory *instance() { 
			if (m_Instance) { return m_Instance; }
			else { return (m_Instance = new XMLDerivedFactory<T, TType, TBase, TBaseType>); }
		}
#endif

		virtual boost::shared_ptr<TBase> create(const ::xml_schema::type &xml_instance) {
			const TType* r = dynamic_cast<const TType*>(&xml_instance);
			if (r) {
				return boost::shared_ptr<TBase>(new T(*r));
			}
			return boost::shared_ptr<TBase>();
		}
		
	};


#endif

} // namespace

#endif
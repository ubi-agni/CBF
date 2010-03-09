/* -*- mode: c-non-suck; -*- */

#ifndef CBF_IMPL_PLUGIN_MACROS_HH
#define CBF_IMPL_PLUGIN_MACROS_HH

#include <cbf/config.h>

#include <cbf/plugin_decl_macros.h>
#include <cbf/debug_macros.h>

#ifdef CBF_HAVE_XSD
	#include <schemas.hxx>
#endif

#ifdef CBF_HAVE_XSD
	/**
		If XSD support is enabled, then add a factory function that create 
		the type from the corresponding XML document type instance.
	*/
	#define CBF_PLUGIN_XSD_CREATOR(ClassName, SuperclassName)\
			boost::shared_ptr<SuperclassName> create(const xsd::cxx::tree::_type &t) {\
				const ClassName##Type *p = dynamic_cast<const ClassName##Type*>(&t); \
				if (p == 0) \
					return boost::shared_ptr<ClassName>(); \
				return boost::shared_ptr<ClassName>(new ClassName(*p)); \
			}
#else
	#define CBF_PLUGIN_XSD_CREATOR(ClassName, SuperclassName)
#endif



#ifdef CBF_HAVE_PLUGIN_SUPPORT
	/**
		This macro creates a subclass of PluginHelper which
		is suitable to be added to the global instance
		of the correct PluginPool class. The constructor
		of this class (defined by CBF_PLUGIN_IMPL_CLASS)
		does the adding automatically...
	*/
	#define CBF_PLUGIN_DECL_CLASS(ClassName,SuperclassName)\
	struct ClassName##PluginHelper : public PluginHelper<SuperclassName> {\
		ClassName##PluginHelper();\
		void *create();\
		CBF_PLUGIN_XSD_CREATOR(ClassName, SuperclassName)\
	};\
	extern ClassName##PluginHelper ClassName##PluginHelperInstance;

#else
	#define CBF_PLUGIN_DECL_CLASS(ClassName,SuperclassName)
#endif

#ifdef CBF_HAVE_PLUGIN_SUPPORT
	#define CBF_PLUGIN_IMPL_CLASS(ClassName,SuperclassName)\
	ClassName##PluginHelper::ClassName##PluginHelper(){\
		m_Name = #ClassName;\
		m_Type = #SuperclassName;\
		CBF_DEBUG(m_Name)\
		CBF_DEBUG(m_Type)\
		PluginPool<SuperclassName>::get_instance()->addPluginHelper(this);\
	}\
	void * ClassName##PluginHelper::create() {\
		return new ClassName;\
	}\
	ClassName##PluginHelper ClassName##PluginHelper##Instance;

	#define CBF_PLUGIN_CLASS(ClassName, SuperclassName)\
		CBF_PLUGIN_DECL_CLASS(ClassName, SuperclassName)\
		CBF_PLUGIN_IMPL_CLASS(ClassName, SuperclassName)
#else
	#define CBF_PLUGIN_CLASS(ClassName, SuperclassName)
	#define CBF_PLUGIN_IMPL_CLASS(ClassName, SuperclassName)
#endif




#endif

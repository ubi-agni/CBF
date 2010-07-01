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

/* -*- mode: c-non-suck; -*- */

#ifndef CBF_IMPL_PLUGIN_MACROS_HH
#define CBF_IMPL_PLUGIN_MACROS_HH

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif


#include <cbf/config.h>

#include <cbf/plugin_decl_macros.h>
#include <cbf/debug_macros.h>


#ifdef CBF_HAVE_XSD
	/**
		If XSD support is enabled, then add a factory function that create 
		the type from the corresponding XML document type instance.
	*/
	#define CBF_PLUGIN_XSD_CREATOR(ClassName, SuperclassName)\
			boost::shared_ptr<SuperclassName> create(const xsd::cxx::tree::_type &t) {\
				try { \
					const ClassName##Type *p = dynamic_cast<const ClassName##Type*>(&t); \
					if (p == 0) \
						return boost::shared_ptr<ClassName>(); \
					return boost::shared_ptr<ClassName>(new ClassName(*p)); \
				} \
				catch (xml_schema::exception &e) { \
					std::cerr << "Error Error Error: " << e.what() << std::endl << std::flush; \
					throw; \
				} \
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
	#define CBF_PLUGIN_DECL_CLASS(ClassName,SuperclassName) \
	struct ClassName##PluginHelper : public PluginHelper<SuperclassName> { \
		ClassName##PluginHelper(); \
		CBF_PLUGIN_XSD_CREATOR(ClassName, SuperclassName) \
	}; \
	extern ClassName##PluginHelper ClassName##PluginHelperInstance;

#else
	#define CBF_PLUGIN_DECL_CLASS(ClassName,SuperclassName)
#endif

#ifdef CBF_HAVE_PLUGIN_SUPPORT
	#define CBF_PLUGIN_IMPL_CLASS(ClassName,SuperclassName) \
	ClassName##PluginHelper::ClassName##PluginHelper(){ \
		m_Name = #ClassName; \
		m_Type = #SuperclassName; \
		CBF_DEBUG(m_Name) \
		CBF_DEBUG(m_Type) \
		PluginPool<SuperclassName>::get_instance()->addPluginHelper(this); \
	} \
	ClassName##PluginHelper ClassName##PluginHelper##Instance;

	#define CBF_PLUGIN_CLASS(ClassName, SuperclassName) \
		CBF_PLUGIN_DECL_CLASS(ClassName, SuperclassName) \
		CBF_PLUGIN_IMPL_CLASS(ClassName, SuperclassName)
#else
	#define CBF_PLUGIN_CLASS(ClassName, SuperclassName)
	#define CBF_PLUGIN_IMPL_CLASS(ClassName, SuperclassName)
#endif




#endif

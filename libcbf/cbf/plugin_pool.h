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

#ifndef CBF_PLUGIN_POOL_HH
#define CBF_PLUGIN_POOL_HH

#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <cbf/config.h>


namespace xsd { namespace cxx { namespace tree {
	class _type;
}}}

namespace CBF {


/**
	The base class for all PluginHelper classes that take care
	of one type each. The derived classes are usually created
	by the macros CBF_PLUGIN_DECL_CLASS and CBF_PLUGIN_IMPL_CLASS
	found in the header cbf/plugin_decl_macros.h and 
	cbf/plugin_impl_macros.h..

	Of the derived classe there is exactly one static instance
	defined in the corresponding source file. This
	instance registers itself with the corresponding
	plugin pool instance.
*/
template <class SuperClass>
struct PluginHelper {
	typedef boost::shared_ptr<SuperClass> SuperClassPtr;
	std::string m_Name;
	std::string m_Type;

	virtual SuperClassPtr create(
		const xsd::cxx::tree::_type &t
	) { 
		return SuperClassPtr(); 
	}

	virtual ~PluginHelper() { }
};


/**
	This class is a template as we need several of these, one for
	each base class of the different controller components.

	There are explcit instantiations of this template for each of
	the required types..

	Each one of them is realized as singleton. Thus to construct a
	certain e.g. SensorTransform from an XML instance type, one would
	do something like

	PluginPool<SensorTransform>::get_instance()->create(...)
*/
template <class SuperClass>
struct PluginPool {
	static PluginPool *get_instance() {
		if (s_Instance == 0) {
			s_Instance = new PluginPool;
			return s_Instance;
		}

		return s_Instance;
	}

	virtual ~PluginPool() {
		s_Instance = 0;
	}

	std::vector<PluginHelper<SuperClass>*> m_PluginHelpers;

	void addPluginHelper(PluginHelper<SuperClass> *plugin_helper) {
		m_PluginHelpers.push_back(plugin_helper);
		#ifdef CBF_DEBUG
			std::cout <<
				"adding plugin with name: " << 
				(plugin_helper->m_Name) << 
				" and type: " << 
				plugin_helper->m_Type << 
				"  and i'm instance: " <<
				(void*)s_Instance <<
				std::endl;
			for (unsigned int i = 0; i < m_PluginHelpers.size(); ++i) {
				CBF_DEBUG("got: " << m_PluginHelpers[i]->m_Name)
			}
		#endif
	}

	boost::shared_ptr<SuperClass> create_from_xml(const xsd::cxx::tree::_type &xml_document);

	template<class XMLType>
	boost::shared_ptr<SuperClass> create_from_file(const std::string &file_name);

	protected:
		static PluginPool *s_Instance;

		PluginPool() {

		}
};


} // namespace

#endif

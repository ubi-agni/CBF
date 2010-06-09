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

#ifndef CBF_COMP_RESOURCE_HH
#define CBF_COMP_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/plugin_decl_macros.h>
#include <cbf/resource.h>

#include <vector>
#include <algorithm>

CBF_PLUGIN_PREAMBLE(CompositeResource)

namespace CBF {

/**
	@brief: A class used to combine several resources into a single one 
*/
struct CompositeResource : public Resource
{
	protected:
		std::vector<ResourcePtr> m_Resources;
		FloatVector m_ResourceValues;

	public:
		CBF_PLUGIN_DECL_METHODS(CompositeResource)

		CompositeResource(std::vector<ResourcePtr> resources = std::vector<ResourcePtr>()) {
			set_resources(resources);
		}

		virtual void set_resources(std::vector<ResourcePtr> resources) {
			m_Resources = resources;
			unsigned int dim = 0;
	
			for (
				unsigned int i = 0, len = m_Resources.size();
				i < len;
				++i)
			{
				dim += m_Resources[i]->dim();
			}
			
			m_ResourceValues = FloatVector(dim);
		}
	
		/**
			@brief Returns the total dimension of the combined resources
		*/
		virtual unsigned int dim() {
			return m_ResourceValues.size();
		}
	
		/**
			@brief Calls update() on the combined resources in the order that 
			they are contained in the m_Resources vector
		*/
		virtual void update() {
			unsigned int current_start_index = 0;
	
			for (
				unsigned int i = 0, len = m_Resources.size();
				i < len;
				++i) 
			{
				m_Resources[i]->update();
	
				std::copy(
					m_Resources[i]->get().begin(), 
					m_Resources[i]->get().end(),
					m_ResourceValues.begin() + current_start_index);
	
				current_start_index += m_Resources[i]->dim();			
			}
		}

		/**
			@brief This method calles the respective methods of the combined resources
		*/	
		virtual void set(const FloatVector& arg) {
			unsigned int current_start_index = 0;
	
			for (
				unsigned int i = 0, len = m_Resources.size();
				i < len;
				++i) 
			{
				FloatVector tmp(m_Resources[i]->dim());
				std::copy(
					arg.begin() + current_start_index,
					arg.begin() + current_start_index + m_Resources[i]->dim(),
					tmp.begin());
	
				m_Resources[i]->set(tmp);
				current_start_index += m_Resources[i]->dim();			
			}
	
		}
	
		/**
			@brief Distributes the passed argument to the combined resources
		*/
		virtual void add(const FloatVector &arg) {
			unsigned int current_start_index = 0;
	
			for (
				unsigned int i = 0, len = m_Resources.size();
				i < len;
				++i) 
			{
				FloatVector tmp(m_Resources[i]->dim());
				std::copy(
					arg.begin() + current_start_index,
					arg.begin() + current_start_index + m_Resources[i]->dim(),
					tmp.begin());
	
				m_Resources[i]->add(tmp);
				current_start_index += m_Resources[i]->dim();			
			}
		}
	
			
		/**
			@brief Returns the cached result.

			See update()
		*/
		virtual FloatVector &get() {
			return m_ResourceValues;
		}
	
};

} // namespace

#endif


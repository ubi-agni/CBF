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
#include <cbf/resource.h>
#include <cbf/namespace.h>

#include <vector>
#include <algorithm>

namespace CBFSchema { class CompositeResource; }

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
		CompositeResource(const CBFSchema::CompositeResource &xml_instance, ObjectNamespacePtr object_namespace);

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

		const std::vector<ResourcePtr> &resources() {
			return m_Resources;
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
	
				/*FIXME:
				std::copy(
					m_Resources[i]->get().begin(), 
					m_Resources[i]->get().end(),
					m_ResourceValues.begin() + current_start_index);
				*/
				m_ResourceValues.segment(current_start_index, m_Resources[i]->get().rows())
						= m_Resources[i]->get();
	
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
				/*FIXME:
				FloatVector tmp(m_Resources[i]->dim());
				std::copy(
					arg.begin() + current_start_index,
					arg.begin() + current_start_index + m_Resources[i]->dim(),
					tmp.begin());
				*/
	
				m_Resources[i]->add(arg.segment(current_start_index, m_Resources[i]->dim()));
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

	typedef boost::shared_ptr<CompositeResource> CompositeResourcePtr;

} // namespace

#endif


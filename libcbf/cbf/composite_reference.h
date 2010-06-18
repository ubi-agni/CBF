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

#ifndef CBF_COMP_REFERENCE_HH
#define CBF_COMP_REFERENCE_HH

#include <cbf/config.h>
#include <cbf/plugin_decl_macros.h>
#include <cbf/reference.h>

#include <vector>
#include <algorithm>

CBF_PLUGIN_PREAMBLE(CompositeReference)

namespace CBF {

/**
	@brief: A class used to combine several references into a single one. 

	Note that this class only combines the first reference, even if one of the wrapped classes 
	provides more than one..
*/
struct CompositeReference : public Reference
{
	protected:
		std::vector<ReferencePtr> m_References;
		std::vector<FloatVector> m_ReferenceValues;

	public:
		CBF_PLUGIN_DECL_METHODS(CompositeReference)

		CompositeReference(std::vector<ReferencePtr> references = std::vector<ReferencePtr>()) {
			set_references(references);
		}

		virtual void set_references(std::vector<ReferencePtr> references) {
			m_References = references;
			unsigned int dim = 0;
	
			m_ReferenceValues.clear();

			for (
				unsigned int i = 0, len = m_References.size();
				i < len;
				++i)
			{
				dim += m_References[i]->dim();
			}
			
			m_ReferenceValues.push_back(FloatVector(dim));
		}
	
		/**
			@brief Returns the total dimension of the combined references
		*/
		virtual unsigned int dim() {
			return m_ReferenceValues[0].size();
		}
	
		/**
			@brief Calls update() on the combined references in the order that 
			they are contained in the m_References vector
		*/
		virtual void update() {
			unsigned int current_start_index = 0;
	
			for (
				unsigned int i = 0, len = m_References.size();
				i < len;
				++i) 
			{
				m_References[i]->update();
	
				std::copy(
					m_References[i]->get()[0].begin(), 
					m_References[i]->get()[0].end(),
					m_ReferenceValues[0].begin() + current_start_index);
	
				current_start_index += m_References[i]->dim();			
			}
		}

			
		/**
			@brief Returns the cached result.

			See update()
		*/
		virtual std::vector<FloatVector> &get() {
			return m_ReferenceValues;
		}
	
};

} // namespace

#endif


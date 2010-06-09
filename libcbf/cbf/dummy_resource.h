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

#ifndef CBF_DUMMY_RESOURCE_HH
#define CBF_DUMMY_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/resource.h>

#include <cbf/plugin_decl_macros.h>


CBF_PLUGIN_PREAMBLE(DummyResource)

#define CBF_HIST_SIZE 750

namespace CBF {
	namespace ublas = boost::numeric::ublas;

	/**
		@brief A resource that does nothing but hold the current value
	*/
	struct DummyResource : public Resource {
		CBF_PLUGIN_DECL_METHODS(DummyResource)

		FloatVector m_Variables;

		/**
			The variance argument can be used to initialize the resource
			with non zero values drawn from a distribution with range
			-variance to variance..

			TODO: implement variance. Right now a fixed range of [-1:1] is 
			used when variance != 0
		*/
		DummyResource(unsigned int variables = 1, Float variance = 0) :
			m_Variables(variables)
		{
			if (variance != 0) {
				for (unsigned int i = 0; i < variables; ++i)
					m_Variables[i] = 2 * M_PI * ((Float)rand()-(RAND_MAX/2.0))/(Float)RAND_MAX;
			}
			//CBF_DEBUG("current values" << m_Variables);
		}
	
	
		~DummyResource() {
			//CBF_DEBUG("Destructor")
		}
	
		virtual const FloatVector &get() {
			return m_Variables;
		}
	
		virtual void set(const FloatVector &arg) {
			m_Variables = arg;
			// CBF_DEBUG("current values" << m_Variables);
		}
	
		virtual void add(const FloatVector &arg);
	
		virtual unsigned int dim() {
			return m_Variables.size();
		}
	};
	
	typedef boost::shared_ptr<DummyResource> DummyResourcePtr;
} // namespace

#endif


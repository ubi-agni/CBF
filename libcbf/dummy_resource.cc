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

#include <cbf/dummy_resource.h>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>
#include <cbf/foreign_object.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <deque>

namespace CBF {

	DummyResource::DummyResource(const FloatVector &values) : Resource()
	{
		m_ResourceValue = FloatVector(values.size());
		m_ResourceValueVelocity = FloatVector(values.size());

		m_ResourceValue = values;
	}

	/**
		The variance argument can be used to initialize the resource
		with non zero values drawn from a distribution with range
		-variance to variance.
	*/
	DummyResource::DummyResource(unsigned int dim, Float variance) : Resource()
	{
		m_ResourceValue = FloatVector(dim);
		m_ResourceValueVelocity = FloatVector(dim);

		if (variance != 0.0) {
			double lfx = 1./sqrt(2.0*M_PI*variance*variance);
			double lx_u;

			for (unsigned int i = 0; i < dim; ++i) {
				lx_u = ((Float)rand()-(RAND_MAX/2.0))/(Float)RAND_MAX;
				m_ResourceValue[i] = lfx * exp(-lx_u/(2.0*variance*variance));
			}
		}
	}

	void DummyResource::read()
	{

	}

	void DummyResource::write(const FloatVector &vel, const Float timestep)
	{
		integrate_Euler(m_ResourceValue, vel, timestep);
		m_ResourceValueVelocity = vel;
	}

	void DummyResource::set(const FloatVector &pos)
	{
		m_ResourceValue = pos;
	}

	void DummyResource::set(const FloatVector &pos, const FloatVector &vel)
	{
		m_ResourceValue = pos;
		m_ResourceValueVelocity = vel;
	}

	#ifdef CBF_HAVE_XSD
		DummyResource::DummyResource(
			const CBFSchema::DummyResource &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			Resource(xml_instance, object_namespace)
		{
			m_ResourceValue =
				*XMLFactory<FloatVector>::instance()->create(
					xml_instance.Vector(), object_namespace
				)
			;

      //CBF_DEBUG("current values: " << m_Variables);
		
		}

		static XMLDerivedFactory<DummyResource, CBFSchema::DummyResource> x;
	#endif
	
} //namespace


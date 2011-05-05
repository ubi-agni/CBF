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

#include <cbf/config.h>
#include <cbf/dummy_reference.h>
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
	DummyReference::DummyReference(
		const CBFSchema::DummyReference &xml_instance, 
		ObjectNamespacePtr object_namespace	
	) : 
		Reference(xml_instance, object_namespace) 
	{
		CBF_DEBUG("constructing");

		//! Set references..
		CBF_DEBUG("Setting Reference(s)...");
		for (
			CBFSchema::DummyReference::Vector_const_iterator it = 
				xml_instance.Vector().begin(); 
			it != xml_instance.Vector().end();
			++it
		)
		{
			FloatVector tmp;

			//CBF_DEBUG("testing XMLFactory")
			//boost::shared_ptr<FloatVector> t = XMLFactory<FloatVector>::instance()->create((CBFSchema::Object &)*it);

			CBF_DEBUG("Extracting reference vector coefficients...");
			try {
				tmp = create_vector(*it, object_namespace);
			} catch(...) {
				CBF_DEBUG("extracting cefficients failed");
				throw;
			}
	
			CBF_DEBUG("Reference: " << tmp);

			m_References.push_back(tmp);
			//PrimitiveControllerPtr controller(new PrimitiveController(*it));
			//m_SubordinateControllers.push_back(controller);
		}
	}

	static XMLDerivedFactory<DummyReference, CBFSchema::DummyReference> x;
	#endif
} // namespace





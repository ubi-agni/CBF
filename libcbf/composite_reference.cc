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

#include <cbf/composite_reference.h>
#include <cbf/debug_macros.h>
#include <cbf/xml_factory.h>

namespace CBF {

	std::vector<FloatVector> &CompositeReference::get() {
		if (m_UpdateSuccessfull == true) {
			CBF_DEBUG("Update success");
			return m_ReferenceValues;
		} else {
			CBF_DEBUG("Update failure");
			return m_EmptyReferenceValues;
		}
	}


#ifdef CBF_HAVE_XSD
	CompositeReference::CompositeReference(const CBFSchema::CompositeReference &xml_instance) {
			std::vector<ReferencePtr> references;
		
			//! Instantiate the subordinate references
			CBFSchema::CompositeReference::Reference1_const_iterator it;
			for (
				it = xml_instance.Reference1().begin(); 
				it != xml_instance.Reference1().end();
				++it
			)
			{
				ReferencePtr tr = XMLBaseFactory<Reference, CBFSchema::Reference>::instance()->create(*it);
				references.push_back(tr);
				//tr->set_reference(ReferencePtr(new DummyReference(tr->get_reference_dim())));
			}
		
			set_references(references);
			CBF_DEBUG("dim: " << dim())
	}

	static XMLDerivedFactory<CompositeReference, CBFSchema::CompositeReference, Reference, CBFSchema::Reference> x;
#endif



} // namespace




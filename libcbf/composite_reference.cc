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
#include <cbf/plugin_impl_macros.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	CBF_PLUGIN_CLASS(CompositeReference, Reference)

	CompositeReference::CompositeReference(const CompositeReferenceType &xml_instance) {
			std::vector<ReferencePtr> references;
		
			//! Instantiate the subordinate references
			CompositeReferenceType::Reference_const_iterator it;
			for (
				it = xml_instance.Reference().begin(); 
				it != xml_instance.Reference().end();
				++it
			)
			{
				ReferencePtr tr(PluginPool<Reference>::get_instance()->create_from_xml(*it));
				references.push_back(tr);
				//tr->set_reference(ReferencePtr(new DummyReference(tr->get_reference_dim())));
			}
		
			set_references(references);
			CBF_DEBUG("dim: " << dim())
	}

#endif

} // namespace




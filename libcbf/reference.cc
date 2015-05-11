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

#include <cbf/reference.h>
#include <cbf/xml_factory.h>

namespace CBF {

  void Reference::set_references(const std::vector<FloatVector> &refs) {
    if (refs.empty())
    {
      CBF_THROW_RUNTIME_ERROR("num_references < 1");
    }

    if (refs[0].size() != m_References[0].size())
    {
      CBF_THROW_RUNTIME_ERROR("dims differ");
    }

    m_References = refs;
    for (unsigned int i = 0; i < refs.size(); ++i)
    {
      CBF_DEBUG("new reference[" << i << "]: " << refs[i]);
    }
  }

  void Reference::set_reference(const FloatVector &ref, const unsigned int ref_index) {
    if (ref.size() != m_References[ref_index].size())
    {
      CBF_THROW_RUNTIME_ERROR("ref dim mismatch");
    }
    if (ref_index >= m_References.size())
    {
      CBF_THROW_RUNTIME_ERROR("ref_index is out of range");
    }

    m_References[ref_index] = ref;
    CBF_DEBUG("new reference: " << ref);
  }

#ifdef CBF_HAVE_XSD
		Reference::Reference(const CBFSchema::Reference &xml_instance, ObjectNamespacePtr object_namespace) :
			Object(xml_instance, object_namespace) 
		{

		}
#endif
} // namespace


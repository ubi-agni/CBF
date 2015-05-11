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

#ifndef CBF_REFERENCE_HH
#define CBF_REFERENCE_HH

#include <cbf/types.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace CBFSchema { class Reference; }

namespace CBF {
	
	/** @brief: Base class for all types of references */
	struct Reference : public Object {

		Reference() : Object("Reference") { }

		Reference(const CBFSchema::Reference &xml_instance, ObjectNamespacePtr object_namespace);
	
		/**
			This function will be called by a controller that uses this reference
			each control cycle. Normally the implementation should just
			update the m_References member..
		*/
		virtual void update() = 0;
	
		virtual unsigned int dim() = 0;

		/**
			A reference can deliver more than one value (e.g. if there is more than one
			task space goal that might be worth reaching. The ambivalence is then
			solved by the potential function (which either just ignores all targets
			but the first or alternatively produces a gradient step in the direction
			of the "closest". The notion of "closest" depends on the space
			the task variable lives in).
		*/
		virtual std::vector<FloatVector> &get() { return m_References; }
	
    virtual void set_references(const std::vector<FloatVector> &refs);

    virtual void set_reference(const FloatVector &ref, const unsigned int ref_index=0);

		virtual ~Reference() { }

		protected:
			/** This member gets updated by update() */
			std::vector<FloatVector> m_References;
	};
	
	
	typedef boost::shared_ptr<Reference> ReferencePtr;
} // namespace

#endif

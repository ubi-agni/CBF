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

#ifndef CONTROL_BASIS_RESOURCE_HH
#define CONTROL_BASIS_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace CBFSchema { class Resource; }

namespace CBF {
	
	/**
		@brief A Resource represents a controller resource for a specific
		robot. 
	
		In the sense of the control basis framework a resource
		must be representable as a vector from R^n. See e.g. the PA10JointResource
		for an example..
	*/
	
	struct Resource : public Object {
		Resource() : Object("Resource") { }

		Resource(const CBFSchema::Resource &xml_instance, ObjectNamespacePtr object_namespace);

		virtual ~Resource() {
	
		}
	
		/**
			This function needs to be called once per cycle. Especially before any
			SensorTransforms using this resource. The default implementation does
			just nothing.
		*/
		virtual void update() {
	
		}
	
		/**
			This function returns the numeric representation of the resource.
			It needs to be implemented by subclasses.. See e.g. the PA10JointResource
			for an example.
		*/
		virtual const FloatVector &get() = 0;
	
		/**
			This function sets the resource from the numerical representation
			as vector (think joint angles of a robot, etc..).. It needs
			to be implemented by subclasses.. See e.g. the PA10JointResource
			for an example.. Note that the resource isn't set to the arg value,
			but rather arg is added to the current value.
		*/
		virtual void add(const FloatVector &arg) = 0;
	
		/**
			The resource's representation is a vector from R^n. This function
			returns the n.
		*/
		virtual unsigned int dim() = 0;
	};

	typedef boost::shared_ptr<Resource> ResourcePtr;
} // namespace

#endif


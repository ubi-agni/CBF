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
#include <cbf/namespace.h>

namespace CBFSchema { class DummyResource; }

namespace CBF {

	/**
		@brief A resource that does nothing but hold the current value
	*/
  struct DummyResource : public Resource {
		DummyResource(const CBFSchema::DummyResource &xml_instance, ObjectNamespacePtr object_namespace);

    DummyResource(const FloatVector &values);

    DummyResource(unsigned int dim, Float variance=0.0);
	
		~DummyResource() {
		}
	
    using Resource::update;

    void update();    

    void add(const FloatVector &resource_velocity, const Float timestep);

    void set(const FloatVector &pos);

    const FloatVector &get_resource_vel();

    const FloatVector &get();
	};
	
	typedef boost::shared_ptr<DummyResource> DummyResourcePtr;
} // namespace

#endif


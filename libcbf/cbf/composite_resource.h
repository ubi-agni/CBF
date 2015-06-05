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

#ifndef CBF_COMP_RESOURCE_HH
#define CBF_COMP_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/resource.h>
#include <cbf/namespace.h>

#include <vector>
#include <algorithm>

namespace CBFSchema { class CompositeResource; }

namespace CBF {

/**
	@brief: A class used to combine several resources into a single one 
*/
struct CompositeResource : public Resource
{
	protected:
		std::vector<ResourcePtr> m_Resources;

	public:
		CompositeResource(const CBFSchema::CompositeResource &xml_instance, ObjectNamespacePtr object_namespace);

    CompositeResource(std::vector<ResourcePtr> resources = std::vector<ResourcePtr>());

    void set_resources(std::vector<ResourcePtr> resources);

    const std::vector<ResourcePtr> &resources();

    /**
      @brief Calls update() on the combined resources in the order that
      they are contained in the m_Resources vector
    */
    virtual void update();

    virtual void add(const FloatVector &resource_velocity, const Float timestep);

    virtual void set(const FloatVector &pos);

    virtual const FloatVector &get_resource_vel();

    virtual const FloatVector &get();

    // set and get the individual resource values
    void set(const unsigned int resource_index, const FloatVector &pos);

    void set(const unsigned int resource_index, const FloatVector &pos, const FloatVector &vel);

    const FloatVector &get(const unsigned int resource_index);

    const FloatVector &get_resource_vel(const unsigned int resource_index);

};

	typedef boost::shared_ptr<CompositeResource> CompositeResourcePtr;

} // namespace

#endif


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
      SensorTransforms using this resource.
      - It reads the state from the hardware.
      - This function must update m_ResourceValue and m_ResourceValueVelocity
    */
    virtual void read() = 0;

    /**
      This function write resource velocity to the hardware.
      Basically cbf output is only resource velocity.
      Hence, each implementation may requires an integration function to compute position
    */
    virtual void write(const FloatVector &vel, const Float timestep) = 0;

    /**
      This function returns current resource postion that is read from the hardware.
    */
    virtual const FloatVector &get_position() {return m_ResourceValue; }

    /**
      This function returns current resource velocity that is read from the hardware.
    */
    virtual const FloatVector &get_velocity() {return m_ResourceValueVelocity; }

    void integrate_Euler(FloatVector &pos,
        const FloatVector &vel,
        const Float timestep);

    void differenciate_Euler(FloatVector &vel,
        const FloatVector &pos,
        const Float timestep);

    unsigned int dim() {return m_ResourceValue.size(); }

    protected:

      /**
        The resource values
      */
      FloatVector m_ResourceValue;
      FloatVector m_ResourceValueVelocity;

	};

	typedef boost::shared_ptr<Resource> ResourcePtr;
} // namespace

#endif


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

#include <cbf/transform_chains.h>
#include <cbf/xml_factories.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
		SensorTransformChain::SensorTransformChain(const CBFSchema::SensorTransformChain &xml_instance) {
			CBF_DEBUG("[SensorTransformChain(const SensorTransformChainType &xml_instance)]: yay!!!");
		
			std::vector<SensorTransformPtr> transforms;
		
			//! Instantiate the subordinate transforms
			CBFSchema::SensorTransformChain::SensorTransform1_const_iterator it;
			for (
				it = xml_instance.SensorTransform1().begin(); 
				it != xml_instance.SensorTransform1().end();
				++it
			)
			{
				SensorTransformPtr tr(XMLBaseFactory<SensorTransform, CBFSchema::SensorTransform>::instance()->create(*it));
				transforms.push_back(tr);
				//tr->set_resource(ResourcePtr(new DummyResource(tr->get_resource_dim())));
			}
		
			set_transforms(transforms);
		}
	#endif
};



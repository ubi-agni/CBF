#include <cbf/transform_chains.h>
#include <cbf/plugin_impl_macros.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
		SensorTransformChain::SensorTransformChain(const SensorTransformChainType &xml_instance) {
			CBF_DEBUG("[SensorTransformChain(const SensorTransformChainType &xml_instance)]: yay!!!");
		
			std::vector<SensorTransformPtr> transforms;
		
			//! Instantiate the subordinate transforms
			SensorTransformChainType::SensorTransform_const_iterator it;
			for (
				it = xml_instance.SensorTransform().begin(); 
				it != xml_instance.SensorTransform().end();
				++it
			)
			{
				SensorTransformPtr tr(PluginPool<SensorTransform>::get_instance()->create_from_xml(*it));
				transforms.push_back(tr);
				//tr->set_resource(ResourcePtr(new DummyResource(tr->get_resource_dim())));
			}
		
			set_transforms(transforms);
		}
	#endif
};



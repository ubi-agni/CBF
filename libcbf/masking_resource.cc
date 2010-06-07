#include <cbf/masking_resource.h>
#include <cbf/plugin_impl_macros.h>
#include <cbf/plugin_pool.h>

namespace CBF {


#ifdef CBF_HAVE_XSD
#ifdef CBF_HAVE_PLUGIN_SUPPORT


MaskingResource::MaskingResource(const MaskingResourceType &xml_instance) {
	ResourcePtr resource 
		= PluginPool<Resource>::get_instance()->create_from_xml(
			xml_instance.Resource());

	std::vector<unsigned int> indexes;

	for (
		MaskingResourceType::Index_const_iterator it = xml_instance.Index().begin();
		it != xml_instance.Index().end();
		++it) {
			indexes.push_back(*it);
	}

	set_resource_and_indexes(resource, indexes);
}


#endif
#endif

CBF_PLUGIN_CLASS(MaskingResource, Resource)


} // namespace

#include <cbf/masking_resource.h>
#include <cbf/plugin_impl_macros.h>
#include <cbf/plugin_pool.h>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

namespace CBF {

#ifdef CBF_HAVE_XSD

MaskingResource::MaskingResource(const CBFSchema::MaskingResource &xml_instance) {
	ResourcePtr resource 
		= PluginPool<Resource>::get_instance()->create_from_xml
			(xml_instance.Resource1());

	std::vector<unsigned int> indexes;

	for 
		(CBFSchema::MaskingResource::Index_const_iterator it = xml_instance.Index().begin();
		 it != xml_instance.Index().end(); 
		 ++it)
	{
			indexes.push_back(*it);
	}

	set_resource_and_indexes(resource, indexes);
}


#endif

CBF_PLUGIN_CLASS(MaskingResource, Resource)


} // namespace

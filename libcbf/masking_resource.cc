#include <cbf/masking_resource.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD

MaskingResource::MaskingResource(const CBFSchema::MaskingResource &xml_instance) {
	ResourcePtr resource 
		= XMLBaseFactory<Resource, CBFSchema::Resource>::instance()->create
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

static XMLDerivedFactory<MaskingResource, CBFSchema::MaskingResource, Resource, CBFSchema::Resource> x;

#endif

} // namespace

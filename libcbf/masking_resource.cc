#include <cbf/masking_resource.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD

MaskingResource::MaskingResource(const CBFSchema::MaskingResource &xml_instance, ObjectNamespacePtr object_namespace) {
	ResourcePtr resource 
		= XMLObjectFactory::instance()->create<Resource>
			(xml_instance.Resource1(), object_namespace);

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

static XMLDerivedFactory<MaskingResource, CBFSchema::MaskingResource> x;

#endif

} // namespace

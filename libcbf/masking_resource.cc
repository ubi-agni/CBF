#include <cbf/masking_resource.h>
#include <cbf/plugin_impl_macros.h>

namespace CBF {


#ifdef CBF_HAVE_XSD
#ifdef CBF_HAVE_PLUGIN_SUPPORT


MaskingResource::MaskingResource(const MaskingResourceType &xml_instance) {

}


#endif
#endif

CBF_PLUGIN_CLASS(MaskingResource, Resource)


} // namespace

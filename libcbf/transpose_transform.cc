#include <cbf/transpose_transform.h>
#include <cbf/plugin_impl_macros.h>

namespace CBF {

CBF_PLUGIN_CLASS(TransposeEffectorTransform, EffectorTransform)

#ifdef CBF_HAVE_XSD
TransposeEffectorTransform::TransposeEffectorTransform(const TransposeEffectorTransformType &xml_instance) {

}
#endif

} // namespace


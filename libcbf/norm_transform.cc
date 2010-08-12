#include <cbf/norm_transform.h>
#include <cbf/plugin_impl_macros.h>
#include <cbf/plugin_pool.h>

namespace CBF {

CBF_PLUGIN_CLASS(NormSensorTransform, SensorTransform)

NormSensorTransform::NormSensorTransform(const NormSensorTransformType &xml_instance) {
	m_Transform = PluginPool<SensorTransform>::get_instance()->create_from_xml(xml_instance.SensorTransform());
	m_Result.resize(1);
	m_TaskJacobian.resize(1,m_Transform->resource_dim());
}

} // namespace

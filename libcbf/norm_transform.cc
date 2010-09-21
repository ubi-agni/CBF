#include <cbf/norm_transform.h>
#include <cbf/xml_factories.h>

namespace CBF {

NormSensorTransform::NormSensorTransform(const ::NormSensorTransform &xml_instance) {
	m_Transform = XMLBaseFactory<SensorTransform, ::SensorTransform>::instance()->create(xml_instance.SensorTransform1());
	m_Result.resize(1);
	m_TaskJacobian.resize(1,m_Transform->resource_dim());
}

} // namespace

#include <cbf/norm_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	NormSensorTransform::NormSensorTransform(const CBFSchema::NormSensorTransform &xml_instance) {
		m_Transform = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.SensorTransform1());
		m_Result.resize(1);
		m_TaskJacobian.resize(1,m_Transform->resource_dim());
	}

	static XMLDerivedFactory<NormSensorTransform, CBFSchema::NormSensorTransform> x;
#endif

} // namespace

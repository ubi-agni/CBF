#include <cbf/norm_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	NormSensorTransform::NormSensorTransform(const CBFSchema::NormSensorTransform &xml_instance) :
		SensorTransform(xml_instance)
	{
		init(XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand()));
	}

	static XMLDerivedFactory<NormSensorTransform, CBFSchema::NormSensorTransform> x;
#endif

} // namespace

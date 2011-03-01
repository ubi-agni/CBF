#include <cbf/norm_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	NormSensorTransform::NormSensorTransform(const CBFSchema::NormSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) :
		SensorTransform(xml_instance, object_namespace)
	{
		init(XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace));
	}

	static XMLDerivedFactory<NormSensorTransform, CBFSchema::NormSensorTransform> x;
#endif

} // namespace

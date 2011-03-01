#include <cbf/qt_sensor_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD

		QtSensorTransform::QtSensorTransform(const CBFSchema::QtSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) :
			SensorTransform(xml_instance, object_namespace)
		{
			init(XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace));
		}

		static XMLDerivedFactory<QtSensorTransform, CBFSchema::QtSensorTransform> x1;
	#endif
}


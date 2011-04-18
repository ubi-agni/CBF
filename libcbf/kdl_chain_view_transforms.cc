#include <cbf/kdl_chain_view_transforms.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	KDLChainViewSensorTransform::KDLChainViewSensorTransform(const CBFSchema::KDLChainViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) {

		SensorTransformPtr s = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace);
	}

	XMLDerivedFactory<KDLChainViewSensorTransform, CBFSchema::KDLChainViewSensorTransform> x;
#endif

} // namespace
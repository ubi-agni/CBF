#include <cbf/kdl_chain_view_transforms.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	KDLChainViewSensorTransform::KDLChainViewSensorTransform(const CBFSchema::KDLChainViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) {

		KDLChainPositionSensorTransformPtr s = 
			XMLObjectFactory::instance()->create<KDLChainPositionSensorTransform>(
				xml_instance.Operand(), object_namespace
			);

		init(s);
	}

	static XMLDerivedFactory<KDLChainViewSensorTransform, CBFSchema::KDLChainViewSensorTransform> x;

	KDLTreeViewSensorTransform::KDLTreeViewSensorTransform(const CBFSchema::KDLTreeViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace) {

		KDLTreePositionSensorTransformPtr s = 
			XMLObjectFactory::instance()->create<KDLTreePositionSensorTransform>(
				xml_instance.Operand(), object_namespace
			);

		init(s);
	}

	static XMLDerivedFactory<KDLTreeViewSensorTransform, CBFSchema::KDLTreeViewSensorTransform> x2;

#endif

} // namespace
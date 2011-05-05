#include <cbf/robotinterface_resource.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	RobotInterfaceResource::RobotInterfaceResource(const CBFSchema::RobotInterfaceResource &xml_instance, ObjectNamespacePtr object_namespace) :
		Resource(xml_instance, object_namespace)
	{
		init(
			xml_instance.SendMemoryName(), 
			xml_instance.RecvMemoryName(), 
			xml_instance.RobotName(), 
			xml_instance.NumberOfJoints()
		);
	}

	static XMLDerivedFactory<RobotInterfaceResource, CBFSchema::RobotInterfaceResource> x;
#endif

} // namespace


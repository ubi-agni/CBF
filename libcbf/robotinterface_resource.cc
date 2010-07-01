#include <cbf/robotinterface_resource.h>
#include <cbf/plugin_impl_macros.h>


namespace CBF {

#ifdef CBF_HAVE_XSD
	CBF_PLUGIN_CLASS(RobotInterfaceResource, Resource)

	RobotInterfaceResource::RobotInterfaceResource(const RobotInterfaceResourceType &xml_instance) {
		init(xml_instance.MemoryName(), xml_instance.RobotName(), xml_instance.NumberOfJoints());
	}

#endif

} // namespace


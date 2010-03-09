#include <cbf/primitive_controller_resource.h>

#include <cbf/debug_macros.h>
#include <cbf/plugin_impl_macros.h>


namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		PrimitiveControllerResource::PrimitiveControllerResource(const PrimitiveControllerResourceType &xml_instance)
		{
			CBF_DEBUG("Creation!!!")
			ControllerPtr p = Controller::create(xml_instance.PrimitiveController());
			m_PrimitiveController = boost::dynamic_pointer_cast<PrimitiveController>(p);
		}	
	#endif
	
	CBF_PLUGIN_CLASS(PrimitiveControllerResource, Resource)
} // namespace


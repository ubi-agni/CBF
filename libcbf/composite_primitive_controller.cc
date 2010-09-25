#include <cbf/composite_primitive_controller.h>

#include <cbf/xml_factory.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
	
		CompositePrimitiveController::CompositePrimitiveController(const CBFSchema::CompositePrimitiveController &xml_instance) 
		{
			
		}
		
		static XMLDerivedFactory<
			CompositePrimitiveController, 
			CBFSchema::CompositePrimitiveController
		> x1;
	#endif
} // namespace


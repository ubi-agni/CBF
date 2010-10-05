#include <cbf/convergence_criterion.h>
#include <cbf/xml_factory.h>
#include <cbf/primitive_controller.h>

namespace CBF {

	bool TaskSpaceDistanceThreshold::check_convergence(const PrimitiveController &p) {
		return false;
	}
	
	bool ResourceStepNormThreshold::check_convergence(const PrimitiveController &p) {
		return false;
	}
	
	
	#ifdef CBF_HAVE_XSD
		TaskSpaceDistanceThreshold::TaskSpaceDistanceThreshold(const CBFSchema::TaskSpaceDistanceThreshold &xml_instance) {

		}

 		ResourceStepNormThreshold::ResourceStepNormThreshold(const CBFSchema::ResourceStepNormThreshold &xml_instance) {

		}

		XMLDerivedFactory<TaskSpaceDistanceThreshold, CBFSchema::TaskSpaceDistanceThreshold> x1;
		XMLDerivedFactory<ResourceStepNormThreshold, CBFSchema::ResourceStepNormThreshold> x2;
	#endif
	
} // namespace 

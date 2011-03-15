#ifndef CBF_CONVERGENCE_CRITERION_HH
#define CBF_CONVERGENCE_CRITERION_HH

#include <cbf/config.h>
#include <cbf/object.h>
#include <cbf/types.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema {
	struct ConvergenceCriterion;
	struct TaskSpaceDistanceThreshold;
	struct ResourceStepNormThreshold;
}

namespace CBF {
	class PrimitiveController;

	struct ConvergenceCriterion : public Object {
		ConvergenceCriterion() : Object("ConvergenceCriterion") { }

		ConvergenceCriterion(const CBFSchema::ConvergenceCriterion &xml_instance, ObjectNamespacePtr object_namespace);

		/**
			@brief An interface for convergence checks in PrimitiveControllers
		
			The primitive controller calls this function each cycle
			and implementations have to use the public interface of the controller
			to find out whether it meets their criterion for convergence..
		*/
		virtual bool check_convergence(const PrimitiveController &p) = 0;
	};
	
	typedef boost::shared_ptr<ConvergenceCriterion> ConvergenceCriterionPtr;



	
	struct TaskSpaceDistanceThreshold : public ConvergenceCriterion {
		Float m_Threshold;
	
		TaskSpaceDistanceThreshold(const CBFSchema::TaskSpaceDistanceThreshold &xml_instance, ObjectNamespacePtr object_namespace);

		TaskSpaceDistanceThreshold(Float threshold) :
			m_Threshold(threshold) { }

		virtual bool check_convergence(const PrimitiveController &p);
	};



	
	struct ResourceStepNormThreshold : public ConvergenceCriterion {
		Float m_Threshold;

		ResourceStepNormThreshold(const CBFSchema::ResourceStepNormThreshold &xml_instance, ObjectNamespacePtr object_namespace);

		ResourceStepNormThreshold(Float threshold) :
			m_Threshold(threshold) { }

		virtual bool check_convergence(const PrimitiveController &p);
	};
} // namespace

#endif


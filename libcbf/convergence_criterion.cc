#include <cbf/convergence_criterion.h>
#include <cbf/xml_object_factory.h>
#include <cbf/primitive_controller.h>

namespace CBF {

	bool TaskSpaceDistanceThreshold::check_convergence(const SubordinateController &p) {
		CBF_DEBUG("check convergence");
		const std::vector<FloatVector> &refs = p.m_Reference->get();

		for (unsigned int i = 0, size = refs.size(); i < size; ++i) {
			CBF_DEBUG("norm of task space distance: " << ublas::norm_2(p.m_CurrentTaskPosition - p.m_Reference->get()[i]));
			if (ublas::norm_2(p.m_CurrentTaskPosition - p.m_Reference->get()[i]) < m_Threshold)
				return true;
		}

		return false;
	}
	
	bool ResourceStepNormThreshold::check_convergence(const SubordinateController &p) {
		CBF_DEBUG("check convergence");

		if (ublas::norm_2(p.m_ResourceStep) < m_Threshold)
			return true;

		return false;
	}
	
	
	#ifdef CBF_HAVE_XSD

		ConvergenceCriterion::ConvergenceCriterion(const CBFSchema::ConvergenceCriterion &xml_instance, ObjectNamespacePtr object_namespace) :
			Object(xml_instance, object_namespace)
		{

		}

		TaskSpaceDistanceThreshold::TaskSpaceDistanceThreshold(
			const CBFSchema::TaskSpaceDistanceThreshold &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			ConvergenceCriterion(xml_instance, object_namespace)
		{
			m_Threshold = xml_instance.Threshold();
			CBF_DEBUG("task space threshold: " << m_Threshold);
		}

 		ResourceStepNormThreshold::ResourceStepNormThreshold(
			const CBFSchema::ResourceStepNormThreshold &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			ConvergenceCriterion(xml_instance, object_namespace)
		{
			m_Threshold = xml_instance.Threshold();
			CBF_DEBUG("resource space threshold: " << m_Threshold);

		}

		XMLDerivedFactory<TaskSpaceDistanceThreshold, CBFSchema::TaskSpaceDistanceThreshold> x1;
		XMLDerivedFactory<ResourceStepNormThreshold, CBFSchema::ResourceStepNormThreshold> x2;
	#endif
	
} // namespace 

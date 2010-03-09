#ifndef CBF_AVOID_SING_HH
#define CBF_AVOID_SING_HH

#include <cbf/primitive_controller.h>

CBF_PLUGIN_PREAMBLE(AvoidSingularitiesController)

namespace CBF {

/**
	This controller is a hack. You only need to specify a sensor transform that
	provides a jacobian. This controller then takes the current resource values,
	wiggles them a bit, reevaluates the jacobian and then determines
	det(J# J) where J is the jacobian, and J# its pseudo inverse.. Note that
	J and J# depend on the current resource values q.
*/
struct AvoidSingularitiesController : public PrimitiveController {
	CBF_PLUGIN_DECL_METHODS(AvoidSingularitiesController)

	FloatMatrix m_JacTimesJacTranspose;

	Float m_SamplingStep;

	AvoidSingularitiesController() :
		m_SamplingStep(0.01) 
	{

	}

	virtual FloatVector &subordinate_step(ublas::vector<Float> &result);
};

} // namespace

#endif


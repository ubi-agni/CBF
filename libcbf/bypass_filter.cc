#include <cbf/bypass_filter.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

void BypassFilter::reset(const FloatVector &state, const FloatVector &state_vel)
{
  m_TargetState = state;
  m_FilteredState = state;

  m_TargetStateVel = state_vel;
  m_FilteredStateVel = state_vel;
}

void BypassFilter::update(
    const FloatVector &state,
    const FloatVector &state_vel,
    const Float timestep)
{
  m_TargetState    = state;
  m_FilteredState    = state;

  m_TargetStateVel = state_vel;
  m_FilteredStateVel = state_vel;
}


  #ifdef CBF_HAVE_XSD
/*
  BypassFilter::BypassFilter(const CBFSchema::BypassFilter &xml_instance, ObjectNamespacePtr object_namespace) :
    Filter(xml_instance, object_namespace)
  {
    CBF_DEBUG("[BypassFilter(const BypassFilter &xml_instance)]: yay!");
    CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());
  }

  static XMLDerivedFactory<
    BypassFilter,
    CBFSchema::PIDTaskSpacePlanner
  > x1;
*/
  #endif

} // namespace

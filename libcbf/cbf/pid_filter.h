#ifndef PID_FILTER_H
#define PID_FILTER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/filter.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class PIDFilter; }

namespace CBF {

  struct PIDFilter : public Filter {
      PIDFilter(const CBFSchema::PIDFilter &xml_instance, ObjectNamespacePtr object_namespace);

      PIDFilter(const Float default_timestep, const unsigned int state_dim, const unsigned int state_vel_dim)  :
        Filter(default_timestep, state_dim, state_vel_dim)
      {        
        set_gain(1.0, 0.0, 0.0);
      }

      virtual void reset(const FloatVector &state, const FloatVector &state_vel);

      virtual void update(
          const FloatVector &state,
          const FloatVector &state_vel,
          const Float timestep);

      void set_gain(const Float gain_p, const Float gain_i=0.0, const Float gain_d=0.0);

    private:
      Float m_GainP, m_GainI, m_GainD;
  };

  typedef boost::shared_ptr<PIDFilter> PIDFilterPtr;

} // namespace


#endif // PID_FILTER_H

#ifndef BYPASS_FILTER_H
#define BYPASS_FILTER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/filter.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class BypassFilter; }

namespace CBF {

  struct BypassFilter : public Filter {
      BypassFilter(const CBFSchema::BypassFilter &xml_instance, ObjectNamespacePtr object_namespace);

      BypassFilter(const Float default_timestep, const unsigned int state_dim, const unsigned int state_vel_dim)  :
        Filter(default_timestep, state_dim, state_vel_dim)
      {        

      }

      virtual void reset(const FloatVector &state, const FloatVector &state_vel);

      virtual void update(
          const FloatVector &state,
          const FloatVector &state_vel,
          const Float timestep);
  };

  typedef boost::shared_ptr<BypassFilter> BypassFilterPtr;

} // namespace


#endif // BYPASS_FILTER_H

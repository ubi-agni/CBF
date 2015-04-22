#ifndef PID_TASK_SPACE_PLANNER_H
#define PID_TASK_SPACE_PLANNER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/task_space_planner.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class PIDTaskSpacePlanner; }

namespace CBF {

  struct PIDTaskSpacePlanner : public TaskSpacePlanner {
      PIDTaskSpacePlanner(const CBFSchema::PIDTaskSpacePlanner &xml_instance, ObjectNamespacePtr object_namespace);

      PIDTaskSpacePlanner(Float timestep, PotentialPtr potential) :
        TaskSpacePlanner(timestep, potential)
      {
        m_Error = FloatVector(potential->dim_grad());
      }

      void reset(const FloatVector &pos, const FloatVector &step);

      void update(const std::vector<FloatVector > &ref);

      void get_task_step(FloatVector &result, const FloatVector &current_pos);

      void set_gain(const Float gain_p, const Float gain_i=0.0, const Float gain_d=0.0);

    private:
      Float m_GainP, m_GainI, m_GainD;
      FloatVector m_Error;
  };

  typedef boost::shared_ptr<PIDTaskSpacePlanner> PIDTaskSpacePlannerPtr;

} // namespace


#endif // PID_TASK_SPACE_PLANNER_H

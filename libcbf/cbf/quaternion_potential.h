#ifndef CBF_QUATERNION_POTENTIAL_H
#define CBF_QUATERNION_POTENTIAL_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class QuaternionPotential; }

namespace CBF {
	
  struct QuaternionPotential : public Potential {
    QuaternionPotential(const CBFSchema::QuaternionPotential &xml_instance, ObjectNamespacePtr object_namespace);

    QuaternionPotential() :
      Potential()
    {

    }

    virtual void gradient (
      FloatVector &result,
      const std::vector<FloatVector > &references,
      const FloatVector &input
    );

    virtual void integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep
    );

    virtual unsigned int sensor_dim() const { return 4u; }

    virtual unsigned int task_dim() const { return 3u; }

		virtual Float distance(const FloatVector &v1, const FloatVector &v2);

    virtual Float norm(const FloatVector &v) { return v.norm(); }
	};

  typedef boost::shared_ptr<QuaternionPotential> QuaternionPotentialPtr;

} // namespace

#endif //CBF_QUATERNION_POTENTIAL_H



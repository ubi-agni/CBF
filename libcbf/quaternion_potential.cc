#include <cbf/quaternion_potential.h>
#include <cbf/quaternion.h>
#include <cbf/utilities.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

  Float QuaternionPotential::distance(const FloatVector &v1, const FloatVector &v2)
  {
    Quaternion q3 = Quaternion(v1) * Quaternion(v2).conjugate();

    Float angle = normalize_angle(acos(q3.w) * 2.0);

    return fabs(angle);
  }

  // result : angular velocity, in R^3
  // input  : quaternion , in R^4 (w, x, y, z)
  void QuaternionPotential::gradient (
    FloatVector &result,
    const std::vector<FloatVector > &references,
    const FloatVector &input)
  {
    //! Find the closest reference
    Float min_distance = distance(references[0], input);
    unsigned int min_index = 0;

    for (unsigned int i = 1; i < references.size(); ++i) {
      Float cur_distance = distance(references[i], input);
      if (cur_distance < min_distance) {
        min_index = i;
        min_distance = cur_distance;
      }
    }

    Quaternion res;
    res = Quaternion(references[min_index])  * Quaternion(input).conjugate();

    result(0) = res.x;
    result(1) = res.y;
    result(2) = res.z;

    if (result.norm() < CBF_QUAT_AXIS_THRESH) {
      result.setZero();
    }
    else {
      result = 2.0* acos(res.w) * result/ result.norm();
    }
  }

  // nextpos    : quaternion at t + timestep, in R^4
  // currentpos : quaternion at t, in R^4
  // currentvel : angular velocity at t, in R^3
  // timestep   : delta t, in sec
  void QuaternionPotential::integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &currentvel,
        const Float timestep)
  {
    Quaternion res;
    FloatVector res_axis(3);

    Float lwnorm = currentvel.norm();

    // 0.00000001 : angular velocity threshold
    if (lwnorm < 0.000000001) {
      nextpos = currentpos;
    }
    else {
      res.w = cos(lwnorm*timestep/2.0);
      res_axis =  sin(lwnorm*timestep/2.0)*currentvel/lwnorm;

      res.x = res_axis(0);
      res.y = res_axis(1);
      res.z = res_axis(2);

      nextpos = res*Quaternion(currentpos);
    }

  }


	#ifdef CBF_HAVE_XSD
    QuaternionPotential::QuaternionPotential(const CBFSchema::QuaternionPotential &xml_instance, ObjectNamespacePtr object_namespace) :
			Potential(xml_instance, object_namespace) {

    }

    static XMLDerivedFactory<QuaternionPotential, CBFSchema::QuaternionPotential> x;
	#endif

} // namespace

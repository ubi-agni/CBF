#include "cbf/line_potential.h"

#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

#include <Eigen/Dense>

namespace CBF {

Float LinePotential::norm(const FloatVector &v)
{
  return v.norm();
}

Float LinePotential::distance(const FloatVector &v1, const FloatVector &v2)
{
  Eigen::Vector3d x0,x1,x2;

  for(int i=0; i<3; i++)
  {
    x0(i) = v1(i);
    x1(i) = v2(i);
    x2(i) = v2(i)+m_LineDirection(i);
  }

  return ((x0-x1).cross(x0-x2)).norm()/(x2-x1).norm();
}

void LinePotential::gradient (
  FloatVector &result,
  const std::vector<FloatVector > &references,
  const FloatVector &input )
{

  Float min_dist = std::numeric_limits<Float>::max();
  unsigned int min_index = 0;

  for (unsigned int i = 0; i < references.size(); ++i) {
    Float dist = distance(input, references[i]);
    if (dist < min_dist) {
      min_index = i;
      min_dist = dist;
    }
  }

  // find a normal vector from the input vector to the line
  FloatVector lProjectionVec(3);
  lProjectionVec = input- references[min_index];

  lProjectionVec = m_LineDirection* m_LineDirection.dot(lProjectionVec);
  result = m_Coefficient * (references[min_index]+lProjectionVec-input);

  Float result_norm = norm(result);

  if (result_norm >= m_MaxGradientStepNorm)
  {
    result = (m_MaxGradientStepNorm/result_norm) * result;
  }
}

void LinePotential::setLineDirection(const FloatVector &v)
{
  m_LineDirection = v;
  m_LineDirection.normalize();
}

} // namespace

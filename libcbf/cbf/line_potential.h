/*
 * line_potential.h
 *
 * It controls the position of the end-effector to be on a line that is consisted
 * with a "reference" point and line direction
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef LINE_POTENTIAL_H
#define LINE_POTENTIAL_H

#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/config.h>
#include <cbf/namespace.h>
#include <kdl/frames.hpp>

#include <boost/shared_ptr.hpp>

namespace CBF {

struct LinePotential : public Potential {
  LinePotential(const LinePotential &xml_instance, ObjectNamespacePtr object_namespace);

  Float m_Coefficient;

  unsigned int m_Dim;
  FloatVector m_LineDirection;

  LinePotential(unsigned int dim = 3, Float coefficient = 1.0) :
    m_Coefficient(coefficient),
    m_Dim(dim)
  {
    m_LineDirection = FloatVector(3);
    m_LineDirection(1) = 1.0;
  }

  virtual Float norm(const FloatVector &v);
  virtual Float distance(const FloatVector &v1, const FloatVector &v2);

  virtual unsigned int dim() const {
    return m_Dim;
  }

  virtual void gradient (
    FloatVector &result,
    const std::vector<FloatVector > &references,
    const FloatVector &input
  );

  void setLineDirection(const FloatVector &v);
};

typedef boost::shared_ptr<LinePotential> LinePotentialPtr;

} // CBF


#endif // LINE_POTENTIAL_H

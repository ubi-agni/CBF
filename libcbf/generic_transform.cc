/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <cbf/generic_transform.h>

#ifndef CBF_HAVE_EIGEN2
  #include <cbf/svd.h>
#endif

namespace CBF {

#ifndef CBF_HAVE_EIGEN2
static const double pseudo_inv_precision_threshold = 0.001;

Float pseudo_inverse(const FloatMatrix &M, FloatMatrix &result) {
  bool transpose = false;

  if (M.size2() > M.size1()) transpose = true;

  FloatMatrix m = M;
  if (transpose){
    m = ublas::trans(M);
  }

  //! Placeholders for the singular value decomposition
  FloatMatrix u(m.size1(), m.size1());
  FloatMatrix v(m.size2(), m.size2());
  FloatMatrix q(m.size2(), m.size1());
  svd(1, 1, 10e-18, 10e-18, m, q, u, v);

  //! Working through the singularValues Matrix calculating the determinant
  Float det = 1.0;
  for (int i = 0; i < m.size2(); ++i) {
    if (fabs(q(i,i)) > pseudo_inv_precision_threshold){
      q(i,i) = 1.0 / (q(i,i));
      det *= q(i,i); ////??????
    } else {
      CBF_DEBUG("SINGULAR")
      q(i,i) = 0.0;
    }
  }

  CBF_DEBUG("deter:" << det)

  CBF_DEBUG("svd: "<< std::endl << q)

  //! Calculating the Moore-Penrose-Pseudoinverse
  FloatMatrix res(v.size1(), q.size2());
  axpy_prod(v, q, res, true);
  std::cout << std::endl;
  FloatMatrix res2(m.size2(), m.size1());
  axpy_prod(res, ublas::trans(u), res2, true);
  
  if (transpose){
    result = ublas::trans(res2);
  } else {
    result = res2;
  }

  return det;
}

Float damped_pseudo_inverse(const FloatMatrix &M, FloatMatrix &result, 
                            Float damping_constant) {
  bool transpose = false;

  if (M.size2() > M.size1()) transpose = true;

  FloatMatrix m = M;
  if (transpose){
    m = ublas::trans(M);
  }

  //! Placeholders for Singular Value Decomposition
  FloatMatrix u(m.size1(), m.size1());
  FloatMatrix v(m.size2(), m.size2());
  FloatMatrix q(m.size2(), m.size1());

  svd(1, 1, 10e-18, 10e-18, m, q, u, v);

  Float det = 1.0;
  //! We use the ordinary reciprocal for testing purposes here
  for (int i = 0; i < m.size2(); ++i) {
    q(i,i) = q(i,i) / (damping_constant + (q(i,i) * q(i,i)));
    det *= q(i,i);
  }

  CBF_DEBUG("deter:" << det)

  CBF_DEBUG("svd: "<< std::endl << q)

  //! Calculating the Moore-Penrose-Pseudoinverse
  FloatMatrix res(v.size1(), q.size2());
  axpy_prod(v, q, res, true);
  FloatMatrix res2(m.size2(), m.size1());
  axpy_prod(res, ublas::trans(u), res2, true);

  if (transpose){
    result = ublas::trans(res2);
  } else {
    result = res2;
  }

  return det;
}
#endif

void GenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}

void DampedGenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	damped_pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian, m_DampingConstant);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}


void DampedWeightedGenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	damped_pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}


#ifdef CBF_HAVE_XSD
	GenericEffectorTransform::GenericEffectorTransform(const ::GenericEffectorTransform &xml_instance)
	{
	
	}
	

	DampedGenericEffectorTransform::DampedGenericEffectorTransform(const ::DampedGenericEffectorTransform &xml_instance)
	{
		m_DampingConstant = (xml_instance.DampingConstant());
	}

	DampedWeightedGenericEffectorTransform::DampedWeightedGenericEffectorTransform(const ::DampedWeightedGenericEffectorTransform &xml_instance)
	{
	
	}
#endif

} // namespace


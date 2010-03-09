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

/* -*- mode: c-non-suck; -*- */

#ifndef CBF_UTILITIES_HH
#define CBF_UTILITIES_HH

#include <cbf/types.h>

#include <cbf/config.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp> 
#include <cmath>

#include <iostream>

namespace KDL {
	class Jacobian;
	class Frame;
}

class VectorType;
class MatrixType;

namespace CBF {

namespace ublas = boost::numeric::ublas;

/** 
	@brief Calculates the result of n-dimensional SLERP.

	Fills result with the result and also returns the given reference..

	NOTE: The vectors should have the same length of 1..
*/
inline FloatVector &slerp(const FloatVector &start, const FloatVector &end, Float step, FloatVector &result) {
	Float angle = acos(ublas::inner_prod(start, end));

	if (angle == 0) {
		result = start;
		return result;
	}

	result = start * (sin((1 - step)* angle)/sin(angle)) + (sin(step * angle)/sin(angle)) * end;

	return result;
}

inline FloatVector unpack_axis_angle3(FloatVector &v) {
	FloatVector ret(4);

	ret[0] = ublas::norm_2(v);

	ret[1] = v[0]/ret[0];
	ret[2] = v[1]/ret[0];
	ret[3] = v[2]/ret[0];

	return ret;
}

/**
	Calculates pseudo inverse of matrix m and writes it into result. m must have more columns
	than rows.
*/
Float pseudo_inverse(const FloatMatrix &m, FloatMatrix &result);

Float weighted_damped_pseudo_inverse(const FloatMatrix &m, FloatMatrix &result);

Float undamped_pseudo_inverse(const FloatMatrix &m, FloatMatrix &result);

template<class matrix_T>
Float determinant(ublas::matrix_expression<matrix_T> const& mat_r)
{
  Float det = 1.0;

  matrix_T mLu(mat_r());
  ublas::permutation_matrix<std::size_t> pivots(mat_r().size1() );

  int is_singular = lu_factorize(mLu, pivots);

  if (!is_singular)
  {
    for (std::size_t i=0; i < pivots.size(); ++i)
    {
      if (pivots(i) != i)
        det *= -1.0;

      det *= mLu(i,i);
    }
  }
  else
    det = 0.0;

  return det;
} 

/** 
	A function to create a ublas::matrix from a KDL::Jacobian. The argument m is 
	filled with a new matrix and a reference to m is returned, too
*/
FloatMatrix &assign(FloatMatrix &m, const KDL::Jacobian &j);

/** 
	A function to create a ublas::matrix from a KDL::Frame. The argument m is 
	filled with a new matrix and a reference to m is returned, too
*/
FloatMatrix &assign(FloatMatrix &m, const KDL::Frame &j);

FloatVector create_vector(const VectorType &xml_instance);
FloatMatrix create_matrix(const MatrixType &xml_instance);

} // namespace


#endif

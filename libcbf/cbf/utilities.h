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
#include <cbf/namespace.h>
#include <cbf/config.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp> 
#include <boost/shared_ptr.hpp>

#include <cmath>

#include <iostream>

namespace KDL {
	class Chain;
	class Tree;
	class Jacobian;
	class Frame;
	class Joint;
	class Segment;
}

namespace CBFSchema {
	class Vector;
	class Matrix;
	class Segment;
	class Frame;
	class Joint;
	class ChainBase;
	class Tree;
}

namespace CBF {

namespace ublas = boost::numeric::ublas;

inline Float normalize_angle(Float angle) {
	Float tmp = fmod(angle + M_PI, 2.0 * M_PI) - M_PI;
	return fmod(tmp - M_PI, 2.0 * M_PI) + M_PI;
}

const Float slerp_threshold = 0.001;
/** 
	@brief Calculates the result of n-dimensional SLERP.

	Fills result with the result and also returns the given reference..

	NOTE: The vectors should have the same length of 1..
*/
FloatVector &slerp(const FloatVector &start, const FloatVector &end, Float step, FloatVector &result);

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
Float damped_pseudo_inverse(const FloatMatrix &m, FloatMatrix &result, Float damping_constant = 0.001);

Float pseudo_inverse(const FloatMatrix &m, FloatMatrix &result);

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

FloatVector create_vector(const CBFSchema::Vector &xml_instance, ObjectNamespacePtr object_namespace);
FloatMatrix create_matrix(const CBFSchema::Matrix &xml_instance, ObjectNamespacePtr object_namespace);

boost::shared_ptr<KDL::Segment> create_segment(const CBFSchema::Segment &xml_instance, ObjectNamespacePtr object_namespace);
boost::shared_ptr<KDL::Frame> create_frame(const CBFSchema::Frame &xml_instance, ObjectNamespacePtr object_namespace);
boost::shared_ptr<KDL::Joint> create_joint(const CBFSchema::Joint &xml_instance, ObjectNamespacePtr object_namespace);

boost::shared_ptr<KDL::Chain> create_chain(const CBFSchema::ChainBase &xml_instance, ObjectNamespacePtr object_namespace);

boost::shared_ptr<KDL::Tree> create_tree(const CBFSchema::Tree &xml_instance, ObjectNamespacePtr object_namespace);


} // namespace


#endif

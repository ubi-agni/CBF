/* -*- mode: c-non-suck; -*- */

#ifndef CBF_TYPES_HH
#define CBF_TYPES_HH

#include <cbf/config.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace CBF {

	/**
		The basic floating point type of libCBF.
	*/
	typedef double Float;
	
	/**
		A typedef to make some other stuff shorter...
	*/
	typedef boost::numeric::ublas::vector<Float> FloatVector;
	typedef boost::numeric::ublas::matrix<Float> FloatMatrix;
	
} // namespace

#endif


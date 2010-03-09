/* -*- mode: c-non-suck; -*- */

#ifndef CBF_REFERENCE_HH
#define CBF_REFERENCE_HH

#include <cbf/types.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <vector>

namespace CBF {
	
	namespace ublas = boost::numeric::ublas;
	
	/** @brief: Base class for all types of references */
	struct Reference {
	
		/**
			This function will be called by a controller that uses this reference
			each control cycle. Normally the implementation shoudl just
			update the m_References member..
		*/
		virtual void update() = 0;
	
		/**
			A reference can deliver more than one value (e.g. if there is more than one
			task space goal that might be worth reaching. The ambivalence is then
			solved by the potential function (which either just ignores all targets
			but the first or alternatively produces a gradient step in the direction
			of the "closest". The notion of "closest" depends on the space
			the task variable lives in).
		*/
		virtual std::vector<FloatVector> &get() { return m_References; }
	
		virtual ~Reference() { }

		protected:
			/** This member gets updated by update() */
			std::vector<FloatVector> m_References;
	};
	
	
	typedef boost::shared_ptr<Reference> ReferencePtr;
} // namespace

#endif

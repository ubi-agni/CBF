#ifndef CBF_ORIENTATION_POTENTIALS_HH
#define CBF_ORIENTATION_POTENTIALS_HH

#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/quaternion.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <vector>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cassert>

CBF_PLUGIN_PREAMBLE(QuaternionPotential)
CBF_PLUGIN_PREAMBLE(AxisAnglePotential)

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	
	
	/**
		@brief A potential function operating on the space of rotations...
	
		...represented as quaternions. This potential implements a gradient step 
		that uses the quaternion SLERP algorithm to calculate a gradient step. E.g. a coefficient
		of 0.01 means that the gradient step is slerp(input, references[0], 0.01).
	*/
	struct QuaternionPotential : public Potential {
		CBF_PLUGIN_DECL_METHODS(QuaternionPotential)
	
		//! @brief  This coefficient determines the rate of convergence. Choose between 0 and 1.
		Float m_Coefficient;

		bool m_Converged;
	
		QuaternionPotential(Float coefficient = 0.1) :
			m_Coefficient(coefficient)
		{
	
		}
	
		virtual void gradient (
			FloatVector &result, 
			const std::vector<FloatVector > &references, 
			const FloatVector &input
		);
	
		virtual unsigned int task_dim() { return 4u; }

		virtual bool converged() { return false; }

		virtual Float norm(const FloatVector &v) {
			Quaternion q = v;
			q.axis_angle();
			return q.w;
		}

		virtual Float distance(const FloatVector &v1, const FloatVector &v2) {
			Quaternion q1; q1.from_axis_angle3(v1);
			Quaternion q2; q2.from_axis_angle3(v2);

			Quaternion q3 = q1.conjugate() * q2;

			//q3.axis_angle();
			// CBF_DEBUG(q3.w);
			return acos(q3.w) * 2.0;
		}
	};

	typedef boost::shared_ptr<QuaternionPotential> QuaternionPotentialPtr;
	
	/**
		@brief A potential function operating on the space of rotations...
	
		represented as Axis Angle, but with all information encoded into the
		direction and length of the axis.
	*/
	struct AxisAnglePotential : public Potential {
		CBF_PLUGIN_DECL_METHODS(AxisAnglePotential)
		//! @brief  This coefficient determines the rate of convergence. Choose between 0 and 1.
		Float m_Coefficient;
		Float m_MaxGradientStepNorm;

		bool m_Converged;
	
		AxisAnglePotential(Float coefficient = 0.1) :
			m_Coefficient(coefficient),
			m_MaxGradientStepNorm(0.1)
		{
	
		}
	
		virtual void gradient (
			FloatVector &result, 
			const std::vector<FloatVector > &references, 
			const FloatVector &input
		);
	
		virtual unsigned int task_dim() { return 3u; }

		virtual bool converged() {
			return m_Converged;
		}

		virtual Float distance(const FloatVector &v1, const FloatVector &v2);

		virtual Float norm(const FloatVector &v) {
			return ublas::norm_2(v);
		}
	};

	typedef boost::shared_ptr<AxisAnglePotential> AxisAnglePotentialPtr;


	/**
		@brief A potential that only cares about the direction of an axis
		and does n-dimensional SLERP to calculate an update step..
	*/
	struct AxisPotential : public Potential {
		bool m_Converged;
		int m_Dimension;
		Float m_Coefficient;

		AxisPotential(int dimension = 3, Float coefficient = 0.1) :
			m_Converged(false),
			m_Dimension(dimension),
			m_Coefficient(coefficient) 
		{

		}

		virtual void gradient (
			FloatVector &result, 
			const std::vector<FloatVector > &references, 
			const FloatVector &input
		) {
			assert(references.size() > 0);

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

			if (min_distance < m_DistanceThreshold)
				m_Converged = true;
			else
				m_Converged = false;

			//! Now that we have the reference closest to our input, calculate 
			//! SLERP step into that direction
			FloatVector ref = 
				(1.0/ublas::norm_2(references[min_index]))
				* references[min_index];

			FloatVector in = 
				(1.0/ublas::norm_2(input))
				* input;

			FloatVector s(m_Dimension);
			slerp(in, ref, m_Coefficient, s);

			result = s - in;
		}
	
		virtual unsigned int task_dim() { return m_Dimension; }

		virtual bool converged() {
			return m_Converged;
		}

		/** 
			@brief This "distance" is the remaining angle between v1 and v2 
		 */
		virtual Float distance(const FloatVector &v1, const FloatVector &v2) {
			FloatVector normed_v1 = v1 * (1.0/ublas::norm_2(v1));
			FloatVector normed_v2 = v2 * (1.0/ublas::norm_2(v2));
			return acos(ublas::inner_prod(normed_v1, normed_v2));
		}

		virtual Float norm(const FloatVector &v) {
			throw std::runtime_error("No sensible definition of norm possible on this space");
			return ublas::norm_2(v);
		}
	};

	typedef boost::shared_ptr<AxisPotential> AxisPotentialPtr;

} // namespace

#endif



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

#ifndef CBF_AXIS_ORIENTATION_POTENTIALS_HH
#define CBF_AXIS_ORIENTATION_POTENTIALS_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/quaternion.h>

#include <boost/shared_ptr.hpp>

CBF_PLUGIN_PREAMBLE(AxisPotential)

namespace CBF {
	namespace ublas = boost::numeric::ublas;

	/**
		@brief A potential that only cares about the direction of an axis
		and does n-dimensional SLERP to calculate an update step..
	*/
	struct AxisPotential : public Potential {
		CBF_PLUGIN_DECL_METHODS(AxisPotential)

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
	
		virtual unsigned int task_dim() const { return m_Dimension; }

		virtual bool converged() const {
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



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

#ifndef CONTROL_BASIS_COMPOSITE_POTENTIAL_HH
#define CONTROL_BASIS_COMPOSITE_POTENTIAL_HH

#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/plugin_decl_macros.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <vector>
#include <cassert>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <iostream>



CBF_PLUGIN_PREAMBLE(CompositePotential)

namespace CBF {

namespace ublas = boost::numeric::ublas;


/**
	@brief The composite potential can be used to combine individual potentials
	into a single one.

	This is useful e.g. when the task space is a product
	of two rather different spaces (position and orientation for example).
*/
struct CompositePotential : public Potential {
	CBF_PLUGIN_DECL_METHODS(CompositePotential)

	/**
		@brief The potentials which are combined into a bigger one..
	*/
	std::vector<PotentialPtr> m_Potentials;

	//! @brief Buffers which are instance variables for efficiency reasons
	std::vector<FloatVector > m_in_buffers;

	//! @brief Buffers which are instance variables for efficiency reasons
	std::vector<FloatVector > m_out_buffers;

	//! @brief Buffers which are instance variables for efficiency reasons
	std::vector<FloatVector > m_ref_buffers;

	unsigned int m_Dim;

	CompositePotential(std::vector<PotentialPtr> potentials = std::vector<PotentialPtr>()) {
		set_potentials(potentials);
	}

	CompositePotential(PotentialPtr p1, PotentialPtr p2) {
		std::vector<PotentialPtr> v;
		v.push_back(p1);
		v.push_back(p2);

		set_potentials(v);
	}

	/**
		@brief Set the potentials to combine..
	*/
	void set_potentials(std::vector<PotentialPtr> &potentials) 
	{
		m_Potentials = potentials;
		m_in_buffers.resize(potentials.size());
		m_out_buffers.resize(potentials.size());
		m_ref_buffers.resize(potentials.size());
		m_Dim = 0;

		for (unsigned int i = 0; i < m_Potentials.size(); ++i) {
			m_Dim += m_Potentials[i]->task_dim();
			m_in_buffers[i] = ublas::zero_vector<Float>(m_Potentials[i]->task_dim());
			m_out_buffers[i] = ublas::zero_vector<Float>(m_Potentials[i]->task_dim());
			m_ref_buffers[i] = ublas::zero_vector<Float>(m_Potentials[i]->task_dim());
		}
	}

	virtual void gradient (
		FloatVector &result, 
		const std::vector<FloatVector > &references, 
		const FloatVector &input
	);

	/**
		We don't know how to combine the norms of the different potentials
		so we return 0..
	*/
	virtual Float norm(const FloatVector &v) {
		throw std::runtime_error("[CompositePotential]: Don't know how to calculate norm");
		return 0;
	}

	virtual Float distance(const FloatVector &v1, const FloatVector &v2) {
		throw std::runtime_error("[CompositePotential]: Don't know how to calculate distance");
		return 0;
	}

	/** 
		This implementation test if all of the potentials are converged and only then
		returns true
	*/
	virtual bool converged() const {
		for (unsigned int i = 0; i < m_Potentials.size(); ++i)
			if (m_Potentials[i]->converged() == false)
				return false;

		return true;
	}

	virtual unsigned int task_dim() const {
		return m_Dim;
	}

};

typedef boost::shared_ptr<CompositePotential> CompositePotentialPtr;

#endif


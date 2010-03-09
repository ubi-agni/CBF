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

#ifndef CBF_COMBINATION_STRATEGY_HH
#define CBF_COMBINATION_STRATEGY_HH

#include <cbf/plugin_decl_macros.h>
#include <cbf/types.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <vector>
#include <string>

CBF_PLUGIN_PREAMBLE(AddingStrategy)

namespace CBF {

namespace ublas = boost::numeric::ublas;

class CombinationStrategy;


/**
	@brief This class is responsible for combining different variable changes
	before they get projected into the nullspace of a controller..
*/
struct CombinationStrategy {
	virtual ~CombinationStrategy() {

	}

	virtual void exec(
		FloatVector &result, 
		const std::vector<FloatVector > &inputs
	) = 0;
};

class AddingStrategy;
typedef boost::shared_ptr<AddingStrategy> AddingStrategyPtr;


/**
	@brief The inputs will be added to the result vector. Make sure you zero the
	result vector beforehand yourself if needed..
*/
struct AddingStrategy : public CombinationStrategy {
	CBF_PLUGIN_DECL_METHODS(AddingStrategy)

	AddingStrategy()
	{

	}

	virtual void exec(
		FloatVector &result, 
		const std::vector<FloatVector > &inputs
	) {
		for (unsigned int i = 0; i < inputs.size(); ++i)
			result += inputs[i];
	}
};

struct WinnerTakesAllStrategy {
	// CBF_PLUGIN_CONSTRUCTOR(WinnerTakesAllStrategy)

};

typedef boost::shared_ptr<CombinationStrategy> CombinationStrategyPtr;

} // namespace

#endif


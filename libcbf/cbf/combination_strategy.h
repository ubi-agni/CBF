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

#include <cbf/types.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>

namespace CBFSchema { 
	class CombinationStrategy;
	class AddingStrategy; 
}

namespace CBF {




/**
	@brief This class is responsible for combining different variable changes
	before they get projected into the nullspace of a controller..
*/
struct CombinationStrategy : public Object {
	CombinationStrategy() : Object("CombinationStrategy") { }
	virtual ~CombinationStrategy() {

	}

	CombinationStrategy(const CBFSchema::CombinationStrategy &xml_instance, ObjectNamespacePtr object_namespace);

	virtual void exec(
		FloatVector &result, 
		const std::vector<FloatVector > &inputs
	) = 0;
};

typedef boost::shared_ptr<CombinationStrategy> CombinationStrategyPtr;


/**
	@brief The inputs will be added to the result vector. Make sure you zero the
	result vector beforehand yourself if needed..
*/
struct AddingStrategy : public CombinationStrategy {
	AddingStrategy (
		const ::CBFSchema::AddingStrategy &xml_instance,
		ObjectNamespacePtr object_namespace
	);

  AddingStrategy(Float weight=1.0) : CombinationStrategy() { m_Weight = weight; }

	virtual void exec(
		FloatVector &result, 
		const std::vector<FloatVector > &inputs
	) {
    for (unsigned int i = 0; i < inputs.size(); ++i) {
			result += inputs[i];
    }
    result *= m_Weight;
	}
  protected:
    Float m_Weight;
};

typedef boost::shared_ptr<AddingStrategy> AddingStrategyPtr;

} // namespace

#endif


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


/* -*- mode: c-non-suck; -*- */

#include <cbf/combination_strategy.h>

#include <boost/numeric/ublas/vector.hpp>

#include <cbf/debug_macros.h>
#include <cbf/plugin_macros.h>

namespace CBF {

	namespace ublas = boost::numeric::ublas;

	#ifdef CBF_HAVE_XSD
		AddingStrategy::AddingStrategy(const AddingStrategyType &xml_instance) {
			CBF_DEBUG("yay!!")
		}
	#endif


	CBF_PLUGIN_CLASS(AddingStrategy, CombinationStrategy)
	// CBF_PLUGIN_CLASS(WinnerTakesAllStrategy, CombinationStrategy)
} // namespace


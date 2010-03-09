#ifndef CBF_DEBUG_MACROS_HH

#include <cbf/config.h>

#include <iostream>
#include <iomanip>
#include <boost/numeric/ublas/io.hpp>

#ifdef CBF_NDEBUG
	#define CBF_DEBUG(arg) ;
#else
	#ifdef CBF_DEBUG_COLOR
		#define CBF_DEBUG(arg) std::cerr << std::fixed << std::setprecision(14) << "\033[22;32m[" << __FILE__ << ":" << __LINE__ << " - \033[22;33m" << __FUNCTION__ << "()\033[22;32m]: \033[22;37m" << arg <<  std::endl;
	#else
		#define CBF_DEBUG(arg) std::cerr << std::fixed << std::setprecision(14) << std::fixed << std::setprecision(8) << """[" << __FILE__ << ":" << __LINE__ << " - " << __FUNCTION__ << "()]: " << arg <<  std::endl;
	#endif
#endif

#endif

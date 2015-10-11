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

#ifndef CBF_DEBUG_MACROS_HH
#define CBF_DEBUG_MACROS_HH

#include <cbf/config.h>

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#ifdef CBF_NDEBUG
	#define CBF_DEBUG(arg) ;
#else
	#ifdef CBF_DEBUG_COLOR
		#define CBF_DEBUG(arg) \
			std::cerr << \
				std::fixed << \
				std::setprecision(8) << \
				"\033[22;32m[" << \
				__FILE__ << \
				":" << \
				__LINE__ << \
				" - \033[22;33m" << \
				__FUNCTION__ << \
				"()\033[22;32m]: \033[22;37m" << \
				arg <<  \
				std::endl << std::flush
	#else
		#define CBF_DEBUG(arg) \
			std::cerr << \
				std::fixed << \
				std::setprecision(8) << \
				"""[" << \
				__FILE__ << \
				":" << \
				__LINE__ << \
				" - " << \
				__FUNCTION__ << \
				"()]: " << \
				arg <<  \
				std::endl << std::flush
	#endif
#endif



#ifdef __GNUC__
	#undef CBF_UNMANGLE
	#include <cxxabi.h>
	#include <string>

	inline std::string gcc_unmangle(const char *typen) {
		int status;
		char *tmp = abi::__cxa_demangle(typen, 0, 0, &status);
		std::string ret(tmp);
		free(tmp);
		return ret;
	}

	#define CBF_UNMANGLE(X) (gcc_unmangle(typeid(X).name()))
#else
	#define CBF_UNMANGLE(X) (typeid(X).name())
#endif

#endif

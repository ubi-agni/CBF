# - If included before targets, sets compiler warning leves to a super pedantic
#   level.
#
# This only works if included after the line containing the PROJECT call in
# CMake.
#
# Copyright (C) 2011 by Arne Nordmann <jwienke at techfak dot uni-bielefeld dot de>
#
# This file may be licensed under the terms of the
# GNU Lesser General Public License Version 3 (the ``LGPL''),
# or (at your option) any later version.
#
# Software distributed under the License is distributed
# on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
# express or implied. See the LGPL for the specific language
# governing rights and limitations.
#
# You should have received a copy of the LGPL along with this
# program. If not, go to http://www.gnu.org/licenses/lgpl.html
# or write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The development of this software was supported by:
#   CoR-Lab, Research Institute for Cognition and Robotics
#     Bielefeld University

INCLUDE(CheckCXXCompilerFlag)

# TODO find a flag that better tells if this is GCC
IF(UNIX)

	SET(FLAGS_TO_TEST 
	  -pipe
	  -Wall
	  -Wextra
	  -fdiagnostics-show-option 
	  -pedantic 
	  -Wchar-subscripts 
	  -Winit-self 
	  -Wswitch-enum 
	  -Wstrict-aliasing=3 
	  -Wstrict-overflow=5 
	  -Wundef 
	  -Wno-endif-labels 
	  -Wshadow 
	  -Wno-system-headers 
	  -Wno-builtin-macro-redefined 
	  -Wmissing-declarations 
	  -Wsign-conversion 
	  -Weffc++)

	FOREACH(F ${FLAGS_TO_TEST})
	  #we make a good unique variable name for the check
	  STRING(REGEX REPLACE "[-+=]" "_" F_CHECK_NAME ${F})
	  SET(F_CHECK_NAME CHECK_CXX_FLAG${F_CHECK_NAME})
	  #now we do the check
	  CHECK_CXX_COMPILER_FLAG(${F} ${F_CHECK_NAME})
	  #if the check pass, lets add the definition
	  IF(${F_CHECK_NAME})
		ADD_DEFINITIONS(${f})
	  ENDIF(${F_CHECK_NAME})
	ENDFOREACH(F ${FLAGS_TO_TEST})

ELSEIF(MSVC)

    # right now W3 is as pedantic as GCC with the above settings
    # still we do not want everything from the compiler...
    
    # needs to have dll-interface to be used by clients
    ADD_DEFINITIONS(/wd4251)
    # exported class derived from non-exported class
    ADD_DEFINITIONS(/wd4275)
    # do not warn for "unsafe" stl functions
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)

ENDIF()

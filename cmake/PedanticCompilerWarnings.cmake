# - If included before targets, sets compiler warning leves to a pedantic but
#   still useful level.
#
# This only works if included after the line containing the PROJECT call in
# CMake.
#
# Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

    CHECK_CXX_COMPILER_FLAG(-pipe CHECK_CXX_FLAG_pipe)
    IF(CHECK_CXX_FLAG_pipe)
        ADD_DEFINITIONS(-pipe)
    ENDIF()
    CHECK_CXX_COMPILER_FLAG(-Wall CHECK_CXX_FLAG_Wall)
    IF(CHECK_CXX_FLAG_Wall)
        ADD_DEFINITIONS(-Wall)
    ENDIF()
    CHECK_CXX_COMPILER_FLAG(-Wextra CHECK_CXX_FLAG_Wextra)
    IF(CHECK_CXX_FLAG_Wextra)
        ADD_DEFINITIONS(-Wextra)
    ENDIF()
    CHECK_CXX_COMPILER_FLAG(-fdiagnostics-show-option CHECK_CXX_FLAG_DIAGNOSTICS)
    IF(CHECK_CXX_FLAG_DIAGNOSTICS)
        ADD_DEFINITIONS(-fdiagnostics-show-option)
    ENDIF()
    # if you are insane, enable this...
    #CHECK_CXX_COMPILER_FLAG(-Weffc++ CHECK_CXX_FLAG_EFFCPP)
    #IF(CHECK_CXX_FLAG_EFFCPP)
    #    ADD_DEFINITIONS(-Weffc++)
    #ENDIF()

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

#IF(MSVC)
#
#    MESSAGE(STATUS "Disabling silly warnings for MSVC")
#
#    ADD_DEFINITIONS(/wd4061) # enum value is not *explicitly* handled in switch
#    ADD_DEFINITIONS(/wd4099) # first seen using 'struct' now seen using 'class'
#    ADD_DEFINITIONS(/wd4127) # conditional expression is constant
#    ADD_DEFINITIONS(/wd4217) # member template isn't copy constructor
#    ADD_DEFINITIONS(/wd4250) # inherits (implements) some member via dominance
#    ADD_DEFINITIONS(/wd4347) # "behavior change", function called instead of template
#    ADD_DEFINITIONS(/wd4355) # "'this': used in member initializer list
#    ADD_DEFINITIONS(/wd4505) # unreferenced function has been removed
#    ADD_DEFINITIONS(/wd4510) # default constructor could not be generated
#    ADD_DEFINITIONS(/wd4511) # copy constructor could not be generated
#    ADD_DEFINITIONS(/wd4512) # assignment operator could not be generated
#    ADD_DEFINITIONS(/wd4513) # destructor could not be generated
#    ADD_DEFINITIONS(/wd4623) # default constructor could not be generated
#    ADD_DEFINITIONS(/wd4624) # destructor could not be generated
#    ADD_DEFINITIONS(/wd4625) # copy constructor could not be generated
#    ADD_DEFINITIONS(/wd4626) # assignment operator could not be generated
#    ADD_DEFINITIONS(/wd4640) # a local static object is not thread-safe
#    ADD_DEFINITIONS(/wd4661) # a member of the template class is not defined.
#    ADD_DEFINITIONS(/wd4670) # a base class of an exception class is inaccessible for catch
#    ADD_DEFINITIONS(/wd4672) # a base class of an exception class is ambiguous for catch
#    ADD_DEFINITIONS(/wd4673) # a base class of an exception class is inaccessible for catch
#    ADD_DEFINITIONS(/wd4675) # resolved overload was found by argument-dependent lookup
#    ADD_DEFINITIONS(/wd4702) # unreachable code, e.g. in <list> header.
#    ADD_DEFINITIONS(/wd4710) # call was not inlined
#    ADD_DEFINITIONS(/wd4711) # call was inlined
#    ADD_DEFINITIONS(/wd4820) # some padding was added
#    ADD_DEFINITIONS(/wd4917) # a GUID can only be associated with a class, interface or namespace
#    ADD_DEFINITIONS(/wd4619) # there is no warning number 'XXXX'
#    ADD_DEFINITIONS(/wd4668) # XXX is not defined as a preprocessor macro
#
#ENDIF()

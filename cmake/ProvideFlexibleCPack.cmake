# - provides a cpack target with a default to TGZ generator and overridable
#   defaults which can be specified in external files. 
#
# INCLUDE this file somewhere at the end of your main CMakeLists.txt. Afterwards
# a package target will be available over CPack and handling is
# available so that different CPack settings can be specified in additional
# files which can be passed in over the CPACK_CONFIG_FILE variable. These files
# should contain definitions of CPACK_* variables.
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

SET(CPACK_GENERATOR "TGZ")
# allow to override packaging system
SET(CPACK_CONFIG_FILE "" CACHE FILEPATH "Path to a CMake lists syntax file overriding settings for CPack so that additional configurations are possible.")
IF(CPACK_CONFIG_FILE)
    INCLUDE(${CPACK_CONFIG_FILE})
ENDIF()

MESSAGE(STATUS "Generating package for generator: ${CPACK_GENERATOR}")

INCLUDE(CPack)

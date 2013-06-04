# - Install files recursively using their header structure in the source tree
# This function install files using their header structure in the source tree.
# This is e.g. usefull to install header files with their structure. Automatic
# support for not mixing up the path logic is provided for generated files
# if they are located in CMAKE_CURRENT_BINARY dir. Otherwise
# CMAKE_CURRENT_SOURCE_DIR is assumed as the absolute location for the files to
# install. 
#
# INSTALL_FILES_RECURSIVE(DESTINATION FILE_LIST)
#
#     DESTINATION: destination folder under which the structure of the files to
#                  install is mirrored. INSTALL is used, hence relative
#                  locations will be relative to CMAKE_INSTALL_PREFIX.
#     FILE_LIST: list of files to install
#
# Exmaple:
# INSTALL_FILES_RECURSIVE(include "rsc/foo.h"
#                                 "rsc/test/bla.h"
#                                 "${CMAKE_CURRENT_BINARY_DIR}/rsc/narf/damn/xxx.h")
# will install:
#   ${CMAKE_INSTALL_PREFIX}/include/rsc/foo.h
#   ${CMAKE_INSTALL_PREFIX}/include/rsc/test/bla.h
#   ${CMAKE_INSTALL_PREFIX}/include/rsc/narf/damn/xxx.h
#
# Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

MACRO(INSTALL_FILES_RECURSIVE DESTINATION FILE_LIST)

    FOREACH(FILE ${${FILE_LIST}})
    
        #MESSAGE("Processing file ${FILE}")
        GET_FILENAME_COMPONENT(ABSOLUTE_FILE ${FILE} ABSOLUTE)
        #MESSAGE("ABSOLUTE_FILE: ${ABSOLUTE_FILE}")
    
        # first, find out if this file is relative to a soruce or binary dir
        FILE(RELATIVE_PATH FILE_REL_TO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} ${ABSOLUTE_FILE})
        FILE(RELATIVE_PATH FILE_REL_TO_BINARY ${CMAKE_CURRENT_BINARY_DIR} ${ABSOLUTE_FILE})
        #MESSAGE("REL_TO_SOURCE: ${FILE_REL_TO_SOURCE}")
        #MESSAGE("REL_TO_BINARY: ${FILE_REL_TO_BINARY}")
        STRING(LENGTH ${FILE_REL_TO_SOURCE} FILE_REL_TO_SOURCE_LENGTH)
        STRING(LENGTH ${FILE_REL_TO_BINARY} FILE_REL_TO_BINARY_LENGTH)
        
        # chose the relative path to start from
        SET(BASE_PATH ${FILE_REL_TO_SOURCE})
        IF(${FILE_REL_TO_BINARY_LENGTH} LESS ${FILE_REL_TO_SOURCE_LENGTH})
            SET(BASE_PATH ${FILE_REL_TO_BINARY})
        ENDIF()
        #MESSAGE("BASE_PATH: ${BASE_PATH}")
    
        # extract destination subdirectory
        GET_FILENAME_COMPONENT(REL_PATH ${BASE_PATH} PATH)
        #MESSAGE("REL_PATH: ${REL_PATH}")
    
        INSTALL(FILES ${FILE} DESTINATION ${DESTINATION}/${REL_PATH})
        
    ENDFOREACH()

ENDMACRO()

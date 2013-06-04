# - Defines several variables forming the version of a software project based
# on a cascade as information sources like the VCS.
#
# DEFINE_PROJECT_VERSION(OUT_VARIABLE_PREFIX
#                        MAJOR_DEFAULT MINOR_DEFAULT PATCH_DEFAULT COMMIT_ID_DEFAULT)
# 
# Defines in the calling scope the variables:
# * ${OUT_VARIABLE_PREFIX}VERSION_MAJOR
# * ${OUT_VARIABLE_PREFIX}VERSION_MINOR
# * ${OUT_VARIABLE_PREFIX}VERSION_PATCH
# * ${OUT_VARIABLE_PREFIX}WC_REVISION
#
# The detailed cascade for defining these variables is as follows (earlier steps
# preceede later ones):
# 1. GIT or SVN-provided information (patch and revision)
# 2. Above-like information provided by the project maintainer found in a
#    vcsversion.cmake file in CMAKE_SOURCE_DIR (patch and revision)
# 3. Defaults provided in the call to this function
#
# Inputs:
#   OUT_VARIABLE_PREFIX: prefix for the defined variables
#   MAJOR_DEFAULT: default major version to use as fallback
#   MINOR_DEFAULT: default minor version to use as fallback
#   PATCH_DEFAULT: default patch version to use as fallback
#   COMMIT_ID_DEFAULT: default commit id to use as fallback
#
# The output variables are not generated in the cmake cache and hence cannot be
# overridden by the user. If this behavior is desired, defined the default
# values in the cache.
#
# To generate a vcsversion.cmake file from git you can use the python script
# git2staticversion.py which is installed along this cmake module.
#
# TODO: still values from the VCS will take preceedence over the defaults, which
# might anyways be a good idea
#
# Copyright (C) 2012 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

INCLUDE(GitProjectVersion)

FUNCTION(DEFINE_PROJECT_VERSION OUT_VARIABLE_PREFIX MAJOR_DEFAULT MINOR_DEFAULT PATCH_DEFAULT COMMIT_ID_DEFAULT)

    # version information
    
    # 1a. try to check git for relevant information
    GIT_PROJECT_VERSION(GIT_TAG_NAME VCS_COMMIT_NUMBER_SINCE_BRANCH VCS_COMMIT_ID)
    
    # 1b. try SVN
    IF(NOT VCS_COMMIT_ID)
        FIND_PACKAGE(Subversion)
        INCLUDE(SubversionIsWc)
        Subversion_IS_WC(${CMAKE_CURRENT_SOURCE_DIR} IS_SVN_VERSION)
        IF(Subversion_FOUND AND IS_SVN_VERSION)
            Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} SVN)
            IF(SVN_WC_REVISION)
                SET(VCS_COMMIT_ID ${SVN_WC_REVISION})
            ENDIF()
        ENDIF()
    ENDIF()
    
    # 2. use a provided version file if existing
    IF(NOT VCS_COMMIT_ID AND EXISTS "${CMAKE_SOURCE_DIR}/gitversion.cmake")
        INCLUDE("${CMAKE_SOURCE_DIR}/vcsversion.cmake")
    ENDIF()
    
    # 3. build variables using defaults
    SET(${OUT_VARIABLE_PREFIX}VERSION_MAJOR "${MAJOR_DEFAULT}" PARENT_SCOPE)
    SET(${OUT_VARIABLE_PREFIX}VERSION_MINOR "${MINOR_DEFAULT}" PARENT_SCOPE)
    # we should not make the next two a cache entry as they wouln't be updated
    # automatically otherwise when the VCS provides new information 
    IF(VCS_COMMIT_NUMBER_SINCE_BRANCH)
        SET(VERSION_PATCH "${VCS_COMMIT_NUMBER_SINCE_BRANCH}")
    ELSE()
        SET(VERSION_PATCH "${PATCH_DEFAULT}")
    ENDIF()
    SET(${OUT_VARIABLE_PREFIX}VERSION_PATCH "${VERSION_PATCH}" PARENT_SCOPE)
    IF(VCS_COMMIT_ID)
        SET(${OUT_VARIABLE_PREFIX}WC_REVISION "${VCS_COMMIT_ID}" PARENT_SCOPE)
    ELSE()
        SET(${OUT_VARIABLE_PREFIX}WC_REVISION "${COMMIT_ID_DEFAULT}" PARENT_SCOPE)
    ENDIF()
    
    SET(${OUT_VARIABLE_PREFIX}VERSION "${MAJOR_DEFAULT}.${MINOR_DEFAULT}.${VERSION_PATCH}" PARENT_SCOPE)

ENDFUNCTION()

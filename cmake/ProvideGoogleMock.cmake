# - Downloads Google Mock (including Google Test) and installs a target that
#   builds a static library from the sources for use in the project.
#
# Will define:
# GMOCK_AVAILABLE - TRUE if Google Mock could be installed and the target is
#                   available. Other variables only have meening if this is TRUE
# GMOCK_INCLUDE_DIRS - Include directories to use the provided gmock installation
# GMOCK_LIBRARIES - Libraries to link tests against
# GMOCK_SOURCES - Internal, sources used for the target
#
# Uses:
# GMOCK_SOURCE - Source URL to download the archive, default exists for 1.6.0.
#                This can also be a file:// URL.
#
# In cases the source URL is changed in the cache, a new download will be
# done.
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

# TODO allow folder location of google mock in addition to the zip archive

SET(GMOCK_SOURCE "http://googlemock.googlecode.com/files/gmock-1.6.0.zip" CACHE STRING "Location to download a google mock zip")
SET(GMOCK_AVAILABLE FALSE CACHE BOOL "Indicates wether a completely extracted installation of gmock is available")

# check whether the download URL has changed. In this case we need to do a new
# download
SET(GMOCK_SOURCE_CHANGED FALSE)
IF(NOT GMOCK_SOURCE STREQUAL GMOCK_SOURCE_CACHE)
    MESSAGE(STATUS "Google Mock download URL changed. Forcing new download.")
    SET(GMOCK_SOURCE_CHANGED TRUE)
ENDIF()

IF(NOT GMOCK_AVAILABLE OR GMOCK_SOURCE_CHANGED)
    
    # find unzip for extracting
    # TODO windows?
    FIND_PROGRAM(UNZIP_COMMAND "unzip")
    IF(NOT UNZIP_COMMAND)
        MESSAGE(WARNING "unzip was not found, Cannot extract the Google Mock archive.")
        RETURN()
    ENDIF()
    
    # download fresh archive
    SET(GMOCK_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/gmock.zip")
    MESSAGE(STATUS "Downloading a fresh Google Mock archive as no old one was found.")
    FILE(DOWNLOAD ${GMOCK_SOURCE} ${GMOCK_ARCHIVE} STATUS GMOCK_DOWNLOAD_STATUS)
    #MESSAGE(STATUS "Google Mock download finished with status: ${GMOCK_DOWNLOAD_STATUS}")
    LIST(GET GMOCK_DOWNLOAD_STATUS 0 GMOCK_DOWNLOAD_STATUS_CODE)
    LIST(GET GMOCK_DOWNLOAD_STATUS 1 GMOCK_DOWNLOAD_STATUS_MESSAGE)
    IF(NOT GMOCK_DOWNLOAD_STATUS_CODE EQUAL 0)
        MESSAGE(WARNING "Download of Google Mock failed: (${GMOCK_DOWNLOAD_STATUS_CODE}) ${GMOCK_DOWNLOAD_STATUS_MESSAGE}")
        FILE(REMOVE ${GMOCK_ARCHIVE})
        RETURN()
    ENDIF()
    
    # find out the name of the top-level folder in the archive and of source files
    EXECUTE_PROCESS(COMMAND ${UNZIP_COMMAND} -l ${GMOCK_ARCHIVE}
                    RESULT_VARIABLE GMOCK_ARCHIVE_CONTENTS_AVAILABLE
                    OUTPUT_VARIABLE GMOCK_ARCHIVE_CONTENTS)
    
    IF(NOT GMOCK_ARCHIVE_CONTENTS_AVAILABLE EQUAL 0)
        MESSAGE(WARNING "Unable to list the contents of the Google Mock archive. Probably the downloaded zip file is corrupted.")
        RETURN()
    ENDIF()                
    
    STRING(REGEX MATCH "[^ ]*/src/gmock-all\\.cc" GMOCK_ALL_FILE ${GMOCK_ARCHIVE_CONTENTS})
    MESSAGE(STATUS "GMOCK_ALL_FILE: ${GMOCK_ALL_FILE}")
    STRING(REGEX MATCH "[^ ]*/src/gtest-all\\.cc" GTEST_ALL_FILE ${GMOCK_ARCHIVE_CONTENTS})
    MESSAGE(STATUS "GMOCK_ALL_FILE: ${GTEST_ALL_FILE}")
    FILE(TO_CMAKE_PATH ${GMOCK_ALL_FILE} GMOCK_ALL_FILE_CMAKE)
    STRING(REPLACE "/src/gmock-all.cc" "" GMOCK_BASE_DIR ${GMOCK_ALL_FILE_CMAKE})
    SET(GMOCK_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${GMOCK_BASE_DIR}")
    MESSAGE(STATUS "GMOCK_BASE_DIR: ${GMOCK_BASE_DIR}")
    
    EXECUTE_PROCESS(COMMAND ${UNZIP_COMMAND} -o ${GMOCK_ARCHIVE} -d ${CMAKE_CURRENT_BINARY_DIR}
                    RESULT_VARIABLE GMOCK_ARCHIVE_EXTRACTED
                    OUTPUT_QUIET)
    IF(NOT GMOCK_ARCHIVE_EXTRACTED EQUAL 0)
        MESSAGE(WARNING "Unable to extract the Google Mock archive. Probably the downloaded zip file is corrupted.")
        RETURN()
    ENDIF()   
    
    # provide usefull information
    SET(GMOCK_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${GMOCK_ALL_FILE}"
                      "${CMAKE_CURRENT_BINARY_DIR}/${GTEST_ALL_FILE}"
                      CACHE STRING "Sources of Google Mock for the compilation target." FORCE)
    
    SET(GMOCK_INCLUDE_DIRS "${GMOCK_BASE_DIR}/gtest"
                           "${GMOCK_BASE_DIR}/gtest/include"
                           "${GMOCK_BASE_DIR}"
                           "${GMOCK_BASE_DIR}/include" CACHE STRING "Google Mock include directories" FORCE)
    SET(GMOCK_LIBRARIES "gmock" CACHE STRING "Library target for Google Mock")
    
    SET(GMOCK_AVAILABLE TRUE CACHE BOOL "Indicates wether a completely extracted installation of gmock is available" FORCE)
    MESSAGE(STATUS "Google Mock successfully installed")

    # store the download URL for later comparison
    SET(GMOCK_SOURCE_CACHE ${GMOCK_SOURCE} CACHE INTERNAL "Cached download URL of gmock for change detection." FORCE)

ELSE()
    MESSAGE(STATUS "Google Mock is already completely available. Skipping download.")
ENDIF()

IF(GMOCK_AVAILABLE)
    
    # declare the target
    INCLUDE_DIRECTORIES(BEFORE SYSTEM ${GMOCK_INCLUDE_DIRS})
    ADD_LIBRARY(${GMOCK_LIBRARIES} STATIC ${GMOCK_SOURCES})

ENDIF()

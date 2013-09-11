# - If included, system dependent information types like lsb_release name, architecture type
#   and some other useful information stored for global CMAKE use
#
#   !!! The content of these variables should not be used in cross compilation projects !!!
#
# Copyright (C) 2011 by Michael Götting  <mgoettin at techfak dot uni-bielefeld dot de>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

SET(LSB_DISTRIBUTOR_ID "unkown")
SET(LSB_RELEASE "unkown")
SET(LSB_CODENAME "unkown")
SET(LSB_BIT_TYPE "unkown")
SET(LSB_ARCH_TYPE "unkown")

# ---- (mgoettin 10/17/2011) TODO: Update this to match all OS ----
SET(LSB_PROCESSOR_ARCH ${CMAKE_SYSTEM_PROCESSOR})

# ---- Get the system bit type ----
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(LSB_BIT_TYPE 64)
ELSE()
  SET(LSB_BIT_TYPE 32)    
ENDIF() 

# ---- Get the system LSB data ----
IF(UNIX)
  
    FIND_PROGRAM(LSB_RELEASE_EXECUTABLE lsb_release)
    IF(LSB_RELEASE_EXECUTABLE)
        # ---- Get the distribution codename ----
        EXECUTE_PROCESS(COMMAND ${LSB_RELEASE_EXECUTABLE} -s -c
          OUTPUT_VARIABLE TMP_LSB_CODENAME
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(TOLOWER ${TMP_LSB_CODENAME} LSB_CODENAME)
        # ---- Get the release name ----
        EXECUTE_PROCESS(COMMAND ${LSB_RELEASE_EXECUTABLE} -s -r
          OUTPUT_VARIABLE TMP_LSB_RELEASE
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(TOLOWER ${TMP_LSB_RELEASE} LSB_RELEASE)
        # ---- Get the distributor id ----
        EXECUTE_PROCESS(COMMAND ${LSB_RELEASE_EXECUTABLE} -s -i
          OUTPUT_VARIABLE TMP_LSB_DISTRIBUTOR_ID
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(TOLOWER ${TMP_LSB_DISTRIBUTOR_ID} LSB_DISTRIBUTOR_ID)
        
        MESSAGE(STATUS "LSB-Release system information::
           LSB Distributor-ID: ${LSB_DISTRIBUTOR_ID}
           LSB Release: ${LSB_RELEASE}
           LSB Codename: ${LSB_CODENAME}
           System bit type: ${LSB_BIT_TYPE} bit")
       
    ENDIF(LSB_RELEASE_EXECUTABLE) 
ENDIF(UNIX)

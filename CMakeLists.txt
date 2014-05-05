cmake_minimum_required(VERSION 2.6)
project(cbf)

set(CBF_VERSION_MAJOR 0)
set(CBF_VERSION_MINOR 2)
set(CBF_VERSION_PATCH 0)
set(CBF_VERSION "${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}")

# use versioned install directories?
set(CBF_VERSIONED_INSTALL 1 CACHE BOOL 
  "use versioned install paths, e.g. include/cbfx.y")
set(CBF_VERSIONED_NAME "${PROJECT_NAME}${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}")
if(CBF_VERSIONED_INSTALL)
  set(CBF_PATH_PREFIX "${CBF_VERSIONED_NAME}")
  set(CBF_PKGCFG_FILE "${CBF_VERSIONED_NAME}.pc")
else(CBF_VERSIONED_INSTALL)
  set(CBF_PATH_PREFIX "")
  set(CBF_PKGCFG_FILE "${PROJECT_NAME}.pc")
endif(CBF_VERSIONED_INSTALL)
set(CBF_LIBRARY_NAME "${PROJECT_NAME}")

message(STATUS "==============================================================")
message(STATUS "Configuring CBF:")

# define default install prefix, if empty
IF(NOT CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Installation directory" FORCE)
  MESSAGE(STATUS "Setting installation directory to ${CMAKE_INSTALL_PREFIX}" )
ENDIF(NOT CMAKE_INSTALL_PREFIX)

# include cmake packages
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CheckIncludeFileCXX)
include(InstallFilesRecursive)
include(FindPkgConfig)
find_package(PkgConfig)
include(ParseArguments)

enable_testing()
include(CTest) 

# define build type
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
  MESSAGE(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'" )
ELSE (NOT CMAKE_BUILD_TYPE)
  MESSAGE(STATUS "Build type set to '${CMAKE_BUILD_TYPE}'" )
ENDIF (NOT CMAKE_BUILD_TYPE)

# and corresponding flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DCBF_NDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCBF_DEBUG_COLOR")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DCBF_NDEBUG")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Python
message(STATUS "Looking for python libs")
find_package(PythonLibs)
if(PYTHONLIBS_FOUND)
  message(STATUS "  found: ${PYTHON_INCLUDE_DIRS} ${PYTHON_LIBRARIES}")
else()
  set(CBF_HAVE_PYTHON 0)
endif()

# Boost libs
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
set(Boost_COMPONENTS "program_options")
if(PYTHONLIBS_FOUND)
  list(APPEND Boost_COMPONENTS "python")
endif()
message(STATUS "Looking for boost components: ${Boost_COMPONENTS}")
find_package(Boost 1.40.0 COMPONENTS ${Boost_COMPONENTS})

set(CBF_HAVE_BOOST ${Boost_FOUND})
set(CBF_HAVE_BOOST_PROGRAM_OPTIONS ${Boost_PROGRAM_OPTIONS_FOUND})
set(CBF_HAVE_PYTHON ${Boost_PYTHON_FOUND})

# cppAD
message(STATUS "Looking for CppAD automatic differentiation package")
find_path(CPPAD_PATH cppad/CppAD.h)
if(NOT ${CPPAD_PATH} STREQUAL "CPPAD_PATH-NOTFOUND")
  message(STATUS "  found: ${CPPAD_PATH}")
  set(CBF_HAVE_CPPAD 1)
else()
  set(CBF_HAVE_CPPAD 0)
endif()

# orocos KDL
message(STATUS "Looking for KDL")
pkg_check_modules(KDL orocos-kdl>=1.0)
if(KDL_FOUND)
  message(STATUS "  found: ${KDL_INCLUDE_DIRS}")
  set(CBF_HAVE_KDL 1)
  link_directories(${KDL_LIBRARY_DIRS})
endif(KDL_FOUND)

# XCF
message(STATUS "Looking for XCF")
pkg_check_modules(XCF xcf>=0.9)
if (XCF_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XCF 1)
endif()

# XRI
message(STATUS "Looking for XRI")
pkg_check_modules(XRI xri)
if (XRI_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XRI 1)
endif()

# xmltio
message(STATUS "Looking for XMLTIO")
pkg_check_modules(XMLTIO xmltio>=1.4)
if (XMLTIO_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XMLTIO 1)
endif()

# ActiveMemory
message(STATUS "Looking for ActiveMemory")
pkg_check_modules(MEMORY Memory>=0.21)
if (MEMORY_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_MEMORY 1)
endif()

# Eigen
if(CBF_HAVE_KDL)
  ## We need to ensure, that we use the same version of Eigen (2 vs. 3) as KDL.
  ## See: https://github.com/norro/CBF/issues/2
  if ("${KDL_INCLUDE_DIRS}" MATCHES ".*eigen3.*")
	 set(CBF_EIGEN_VERSION 3)
	 add_definitions("-DEIGEN2_SUPPORT")
  else()
	 set(CBF_EIGEN_VERSION 2)
  endif()
  set(CBF_HAVE_EIGEN 1)
  include_directories(${KDL_INCLUDE_DIRS})
  message(STATUS "Using Eigen${CBF_EIGEN_VERSION} from KDL: ${KDL_INCLUDE_DIRS}")
else(CBF_HAVE_KDL)
  ## If we don't have KDL, we are free to choose Eigen
  message(STATUS "Looking for Eigen3")
  find_package(Eigen3)
  if(EIGEN3_FOUND)
    message(STATUS "  ${EIGEN3_VERSION} found.")
    set(CBF_HAVE_EIGEN 1)
	 set(CBF_EIGEN_VERSION 3)
	 add_definitions("-DEIGEN2_SUPPORT")
    include_directories(${EIGEN3_INCLUDE_DIR})
  else()
    message(STATUS "Looking for Eigen2")
    find_package(Eigen2)
    if(EIGEN2_FOUND)
      set(CBF_HAVE_EIGEN 1)
		set(CBF_EIGEN_VERSION 2)
      message(STATUS "  ${EIGEN2_VERSION} found")
      include_directories(${EIGEN2_INCLUDE_DIR})
    endif()
  endif()
endif(CBF_HAVE_KDL)

# Qt
message(STATUS "Looking for QT4")
pkg_check_modules(QT QtCore QtGui)
if(QT_FOUND)
  message(STATUS "  found")
  find_package(Qt4)
  include(FindQt4)
  set(CBF_HAVE_QT 1)
endif()

# libqkdlview
message(STATUS "Looking for qkdlview")
find_library(QKDLVIEW_LIB qkdlview)
find_path(QKDLVIEW_INCLUDES qkdltreeview.h)
if(QKDLVIEW_LIB AND QKDLVIEW_INCLUDES)
  message(STATUS "  found qkdlview ${QKDLVIEW_LIB} ${QKDLVIEW_INCLUDES}")
  set(CBF_HAVE_QKDLVIEW 1)
endif()

# spacemouse
message(STATUS "Looking for spacemouse")
find_library(SPACEMOUSE_LIBS spacenavi)
find_path(SPACEMOUSE_INCLUDES spacenavi.h)
if(
	 NOT "${SPACEMOUSE_LIBS}" STREQUAL "SPACEMOUSE_LIBS-NOTFOUND" 
	 AND 
	 NOT "${SPACEMOUSE_INCLUDES}" STREQUAL "SPACEMOUSE_INCLUDES-NOTFOUND"
	 )
  message(STATUS "  found spacemouse ${SPACEMOUSE_LIBS} ${SPACEMOUSE_INCLUDES}")
  set(CBF_HAVE_SPACEMOUSE 1)
endif()

# xerces: required for header-only xsd
message(STATUS "Looking for Xerces-C")
find_library(XERCESC_LIB xerces-c)
find_path(XERCESC_INC xercesc/dom/DOM.hpp)
if(NOT "${XERCESC_LIB}" STREQUAL "XERCESC_LIB-NOTFOUND" AND NOT "${XERCESC_INC}" STREQUAL "XERCESC_INC-NOTFOUND")
  message(STATUS "  found Xerces-C ${XERCESC_LIB} ${XERCESC_INC}")
  set(CBF_HAVE_XERCESC 1)
endif()

message(STATUS "Looking for xmlstarlet")
find_program(XMLSTARLET_BIN NAMES xmlstarlet)

message(STATUS "Looking for XSD CXX")
find_program(XSD_BIN NAMES xsdcxx xsd)
find_path(XSD_INC xsd/cxx/config.hxx)
if (NOT "${XSD_BIN}" STREQUAL "XSD_BIN-NOTFOUND" AND NOT "${XSD_INC}" STREQUAL "XSD_INC-NOTFOUND" )
  message(STATUS "  found XSD CXX: ${XSD_BIN} ${XSD_INC}")
  if(CBF_HAVE_XERCESC)
	 set(CBF_HAVE_XSD 1)
  endif()
endif()

message(STATUS "Looking for pyxbgen")
find_program(PYXBGEN_BIN NAMES pyxbgen)
if (NOT "${PYXBGEN_BIN}" STREQUAL "PYXBGEN_BIN-NOTFOUND")
  message(STATUS "  found pyxbgen: ${PYXBGEN_BIN}")
  set(CBF_HAVE_PYXBGEN 1)
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
  )

configure_file(
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_BINARY_DIR}/Doxyfile"
  ) 

# --- pkgconfig file ---
configure_file(cbf.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc 
        DESTINATION lib/pkgconfig
		  RENAME ${CBF_PKGCFG_FILE}
		  )

# --- cmake config file ---
configure_file(CBFConfig.cmake.in ${CMAKE_BINARY_DIR}/CBFConfig.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CBFConfig.cmake 
        DESTINATION share/${CBF_PATH_PREFIX})
configure_file(CBFConfigVersion.cmake.in ${CMAKE_BINARY_DIR}/CBFConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CBFConfigVersion.cmake 
        DESTINATION share/${CBF_PATH_PREFIX})
        
add_subdirectory(schemas)
add_subdirectory(libcbf)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(doc)

EXPORT(TARGETS ${CBF_LIBRARY_NAME} ${CBF_LIBRARY_NAME}_schemas 
       FILE "${CMAKE_BINARY_DIR}/CBFDepends.cmake")
INSTALL(EXPORT CBFDepends
        DESTINATION "share/${CBF_PATH_PREFIX}")

# --- package ---
set(CPACK_PACKAGE_VERSION_MAJOR ${CBF_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CBF_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CBF_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "CoR-Lab Bielefeld University")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")

include(ProvideFlexibleCPack)


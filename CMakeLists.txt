cmake_minimum_required(VERSION 2.6)

project(CBF)

set(CBF_VERSION_MAJOR 0)
set(CBF_VERSION_MINOR 1)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD OFF)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -DCBF_NDEBUG")

find_package(Boost 1.34.1 COMPONENTS program_options python)

if(Boost_FOUND)
  set(CBF_HAVE_BOOST 1)
  set(CBF_HAVE_BOOST_PROGRAM_OPTIONS 1)
else()
  message(FATAL_ERROR "boost and boost_program_options are required for CBF")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(PythonInterp "Found Python" python2)

find_package(PkgConfig)

pkg_check_modules(KDL orocos-kdl>=1.0)
if (KDL_FOUND)
  set(CBF_HAVE_KDL 1)
endif()

pkg_check_modules(XCF xcf>=0.9)
if (XCF_FOUND)
  set(CBF_HAVE_XCF 1)
endif()

pkg_check_modules(XMLTIO xmltio>=1.4)
if (XMLTIO_FOUND)
  set(CBF_HAVE_XMLTIO 1)
endif()

pkg_check_modules(MEMORY Memory>=0.21)
if (MEMORY_FOUND)
  set(CBF_HAVE_MEMORY 1)
endif()

pkg_check_modules(EIGEN2 eigen2)
if (EIGEN2_FOUND)
  set(CBF_HAVE_EIGEN2 1)
endif()

#find_path(EIGEN2_INC eigen2/Eigen/Core)
#if (NOT "${EIGEN2_INC}" STREQUAL "EIGEN2_INC-NOTFOUND")
#  set(CBF_HAVE_EIGEN2 1)
#endif()

message(STATUS "Looking for Xerces-C")
find_library(XERCESC_LIB xerces-c)
find_path(XERCESC_INC xercesc/dom/DOM.hpp)
if(NOT "${XERCESC_LIB}" STREQUAL "XERCESC_LIB-NOTFOUND" AND NOT "${XERCESC_INC}" STREQUAL "XERCESC_INC-NOTFOUND")
  message(STATUS "  found Xerces-C ${XERCESC_LIB} ${XERCESC_INC}")
  set(CBF_HAVE_XERCESC 1)
endif()


message(STATUS "Looking for XSD CXX")
find_program(XSD_BIN NAMES xsd xsdcxx)
find_path(XSD_INC xsd/cxx/config.hxx)
if (NOT "${XSD_BIN}" STREQUAL "XSD_BIN-NOTFOUND" AND NOT "${XSD_INC}" STREQUAL "XSD_INC-NOTFOUND" )
  message(STATUS "  found XSD CXX: ${XSD_BIN} ${XSD_INC}")
  if(CBF_HAVE_XERCESC)
	 set(CBF_HAVE_XSD 1)
  endif()
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
  )

add_subdirectory(schemas)
add_subdirectory(libcbf)
add_subdirectory(apps)

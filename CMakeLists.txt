cmake_minimum_required(VERSION 2.6)

project(CBF)

set(CBF_VERSION_MAJOR 0)
set(CBF_VERSION_MINOR 1)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD OFF)

find_package(Boost 1.34.1 COMPONENTS program_options)

if(NOT Boost_FOUND)
  message(FATAL_ERROR "boost is required for CBF")
endif(NOT Boost_FOUND)

find_package(PkgConfig)
pkg_check_modules(KDL orocos-kdl>1.0)
pkg_check_modules(XCF xcf>=0.9)
pkg_check_modules(XMLTIO xmltio>=1.4)
pkg_check_modules(MEMORY Memory>=0.21)

find_path(
  EIGEN2_INC
  Eigen/Core
)

find_program(XSD_BIN NAMES xsd xsdcxx)
find_path(XSD_INC xsd/cxx/config.hxx)

if (NOT "${XSD_BIN}" STREQUAL "XSD_BIN-NOTFOUND" AND NOT "${XSD_INC}" STREQUAL "XSD_INC-NOTFOUND" )
  set(CBF_HAVE_XSD 1)
  include_directories("${XSD_INC}")
endif ()

configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
)

if (CBF_HAVE_XSD)
  add_subdirectory(schemas)
endif()

add_subdirectory(libcbf)



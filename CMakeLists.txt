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

find_library(
  EIGEN2 
  eigen2
)

add_subdirectory(schemas)



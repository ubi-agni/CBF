if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Cannot configure CPack to generate Debian packages on non-linux systems.")
endif()

include(CheckLSBTypes)

#set(CPACK_INSTALL_PREFIX "/usr")
set(CPACK_SET_DESTDIR On)
set(CMAKE_INSTALL_PREFIX ${CPACK_INSTALL_PREFIX})

set(PACKAGE_NAME "nemomath")
set(VERSION_SUFFIX "${NEMOMATH_VERSION_MAJOR}.${NEMOMATH_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${CPACK_PACKAGE_REVISION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}${VERSION_SUFFIX}-${CPACK_PACKAGE_VERSION}_${LSB_CODENAME}_${LSB_PROCESSOR_ARCH}")
set(METAPACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}_${LSB_CODENAME}_${LSB_PROCESSOR_ARCH}")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME "${PACKAGE_NAME}${VERSION_SUFFIX}")
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matthias Rolf <mrolf@cor-lab.uni-bielefeld.de>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Nemomath
 A header-only template library for mathematical computations in the context of robotics and learning.")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libeigen2-dev | libeigen3-dev, libboost-dev, libboost-test-dev")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "libeigen2-dev")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "libeigen3-dev")

# Configure meta package configuration files
configure_file(cmake/metapackage.conf.in ${CMAKE_BINARY_DIR}/metapackage/DEBIAN/control @ONLY)
configure_file(cmake/metapackage_build.sh.in ${CMAKE_BINARY_DIR}/metapackage_build.sh)

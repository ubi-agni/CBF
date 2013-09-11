set(NEMOMATH_CMAKE_MODULE_PATH "share/${NEMOMATH_PATH_PREFIX}/cmake/Modules")
message(STATUS "Installing CMake modules to: ${NEMOMATH_CMAKE_MODULE_PATH}")

install(FILES
            FindEigen2.cmake
            FindEigen3.cmake
        DESTINATION ${NEMOMATH_CMAKE_MODULE_PATH})

include(ExternalProject)
ExternalProject_Add(
        hermes PREFIX hermes
        URL "https://github.com/filipecn/hermes/archive/refs/heads/main.zip"
        # URL_HASH SHA1=fe17a0610a239311a726ecabcd2dbd669fb24ca8
        CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DBUILD_WITH_CUDA=OFF"
        "-DINSTALL_PATH=install"
        #        CMAKE_CACHE_ARGS
        #        "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
        #        "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"

)

ExternalProject_Get_Property(hermes INSTALL_DIR)
set(INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/hermes)
set(HERMES_INCLUDES
        ${INSTALL_DIR}/install/include
        )
set(HERMES_LIBRARIES ${INSTALL_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}hermes${CMAKE_STATIC_LIBRARY_SUFFIX})


set(HERMES_INCLUDES /mnt/windows/Projects/hermes)
set(HERMES_LIBRARIES /mnt/windows/Projects/hermes/cmake-build-release/libhermes.a)


set(HERMES_INCLUDES ${HERMES_INCLUDES} CACHE STRING "")
set(HERMES_LIBRARIES ${HERMES_LIBRARIES} CACHE STRING "")
include(ExternalProject)

include(ExternalProject)
ExternalProject_Add(
        tinyobj PREFIX tinyobj
        URL "https://github.com/tinyobjloader/tinyobjloader/archive/master.zip"
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/ext_build/tinyobj-src"
        BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/ext_build/tinyobj-build"
        CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        CMAKE_CACHE_ARGS
        "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(tinyobj INSTALL_DIR)

set(TINYOBJ_INCLUDE_DIR ${INSTALL_DIR}/include)
set(TINYOBJ_LIBRARIES ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}tinyobjloader${CMAKE_STATIC_LIBRARY_SUFFIX})

set(TINYOBJ_INCLUDE_DIR ${TINYOBJ_INCLUDE_DIR} CACHE STRING "")
set(TINYOBJ_LIBRARIES ${TINYOBJ_LIBRARIES} CACHE STRING "")

install(FILES
        ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}tinyobjloader${CMAKE_STATIC_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_PATH}/lib
        )

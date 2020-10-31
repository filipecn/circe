include(ExternalProject)

set(BUILD_TYPE "Release")
if (MSVC)
    set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif (MSVC)

ExternalProject_Add(
        spdlog PREFIX spdlog
        URL "https://github.com/gabime/spdlog/archive/v1.x.zip"
        CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
        CMAKE_CACHE_ARGS
        "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
)


ExternalProject_Get_Property(spdlog INSTALL_DIR)
set(SPDLOG_INCLUDE_DIR ${INSTALL_DIR}/include)

set(SUFFIX "d")
if (BUILD_TYPE MATCHES "Release")
    set(SUFFIX "")
endif (BUILD_TYPE MATCHES "Release")

set(SPDLOG_LIBRARIES ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spdlog${SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SPDLOG_INCLUDE_DIR ${SPDLOG_INCLUDE_DIR} CACHE STRING "")
set(SPDLOG_LIBRARIES ${SPDLOG_LIBRARIES} CACHE STRING "")

install(DIRECTORY
        ${INSTALL_DIR}/include/spdlog
        DESTINATION ${INSTALL_PATH}/include
        FILES_MATCHING REGEX "(.*\\.[inl|h])")

install(FILES
        ${SPDLOG_LIBRARIES}
        DESTINATION ${INSTALL_PATH}/lib)

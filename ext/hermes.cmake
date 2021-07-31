if (HERMES_INCLUDE_PATH AND HERMES_LIB_PATH)
    MESSAGE("using hermes from " ${HERMES_INCLUDE_PATH} " and " ${HERMES_LIB_PATH})
    set(HERMES_INCLUDES ${HERMES_INCLUDE_PATH})
    set(HERMES_LIBRARIES ${HERMES_LIB_PATH})
    add_library(hermes STATIC ${HERMES_LIB_PATH})
    if (BUILD_WITH_CUDA)
        set_target_properties(hermes PROPERTIES
                LINKER_LANGUAGE CUDA)
    else ()
        set_target_properties(hermes PROPERTIES
                LINKER_LANGUAGE CXX)
    endif ()
else ()
    include(ExternalProject)
    ExternalProject_Add(
            hermes PREFIX hermes
            URL "https://github.com/filipecn/hermes/archive/refs/heads/main.zip"
            # URL_HASH SHA1=fe17a0610a239311a726ecabcd2dbd669fb24ca8
            CMAKE_ARGS
            "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DBUILD_WITH_CUDA=${BUILD_WITH_CUDA}"
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
endif ()

set(HERMES_INCLUDES ${HERMES_INCLUDES} CACHE STRING "")
set(HERMES_LIBRARIES ${HERMES_LIBRARIES} CACHE STRING "")
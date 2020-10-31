project(GLAD)

cmake_minimum_required(VERSION 2.8.7)

add_library(glad STATIC glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(UNIX)
        target_link_libraries(glad -ldl)
endif(UNIX)

set_target_properties(glad PROPERTIES
        OUTPUT_NAME "glad"
        FOLDER "GLAD")

install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/glad
        ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR
        DESTINATION ${INSTALL_PATH}/include
        FILES_MATCHING REGEX "(.*\\.[inl|h])")
install(TARGETS glad
        LIBRARY DESTINATION ${INSTALL_PATH}/lib
        ARCHIVE DESTINATION ${INSTALL_PATH}//lib
        )

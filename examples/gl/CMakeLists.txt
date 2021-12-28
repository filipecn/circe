# extracted from https://github.com/SaschaWillems/Vulkan/blob/master/examples/CMakeLists.txt
function(buildExample EXAMPLE_NAME)
    set(EXAMPLE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME})
    message(STATUS "Generating project file for example in ${EXAMPLE_FOLDER}")
    # Main
    SET(MAIN_CPP ${EXAMPLE_NAME}.cpp)
    # Add shaders
    set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}/shaders")
    file(GLOB SHADERS
            "${SHADER_DIR}/*.vert"
            "${SHADER_DIR}/*.frag"
            "${SHADER_DIR}/*.comp"
            "${SHADER_DIR}/*.geom"
            "${SHADER_DIR}/*.tesc"
            "${SHADER_DIR}/*.tese"
            "${SHADER_DIR}/*.mesh"
            "${SHADER_DIR}/*.task"
            "${SHADER_DIR}/*.rgen"
            "${SHADER_DIR}/*.rchit"
            "${SHADER_DIR}/*.rmiss"
            )
    add_executable(${EXAMPLE_NAME} ${MAIN_CPP} ${SHADERS})
    add_dependencies(${EXAMPLE_NAME} glfw hermes circe)

    target_link_libraries(${EXAMPLE_NAME}
            circe
            ${HERMES_LIBRARIES}
            ${CIRCE_LIBRARIES}
            ${GLFW_LIBRARIES}
            ${TINYOBJ_LIBRARIES}
            )
    target_include_directories(${EXAMPLE_NAME} PUBLIC
            ${STB_INCLUDES}
            ${GLFW_INCLUDES}
            ${VULKAN_INCLUDES}
            ${HERMES_INCLUDE_DIR}
            )
    target_compile_definitions(${EXAMPLE_NAME} PUBLIC
            -DSHADERS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders"
            -DMODELS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/models"
            -DTEXTURES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
            )
endfunction(buildExample)

set(EXAMPLES
        2d
        instances
        hello_circe
        shadows
        skybox
        picking
#        texture_atlas
        deferred_rendering
        volume_texture
        viewports

#        volume_box
#        volume_box_2d
#        compute_shader
#        save_viewport
#        scene_mesh_example
#        post_effects
#        text
#        scene_object_interaction
#        compiling_shaders
        ssbo
#        mesh_editor
        )

foreach (EXAMPLE ${EXAMPLES})
    buildExample(${EXAMPLE})
endforeach (EXAMPLE)
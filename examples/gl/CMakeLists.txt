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
    add_dependencies(${EXAMPLE_NAME} glfw ponos circe)
    target_link_libraries(${EXAMPLE_NAME}
            circe
            ${PONOS_LIBRARIES}
            ${CIRCE_LIBRARIES}
            ${GLFW_LIBRARIES}
            ${TINYOBJ_LIBRARIES}
            )
    target_include_directories(${EXAMPLE_NAME} PUBLIC
            ${STB_INCLUDES}
            ${GLFW_INCLUDES}
            ${VULKAN_INCLUDES}
            ${PONOS_INCLUDE_DIR}
            )
    target_compile_definitions(${EXAMPLE_NAME} PUBLIC
            -DSHADERS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders"
            -DMODELS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/models"
            -DTEXTURES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
            )
endfunction(buildExample)

set(EXAMPLES
        hello_circe
        volume_box
        volume_box_2d
        compute_shader
        save_viewport
        viewports
        instances
        scene_mesh_example
        post_effects
        text
        scene_object_interaction
        shader_editor
        compiling_shaders
        )

foreach (EXAMPLE ${EXAMPLES})
    buildExample(${EXAMPLE})
endforeach (EXAMPLE)
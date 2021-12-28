# avoid policy warning message
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
# main project
project(CIRCE)
cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)
set(CMAKE_VERBOSE_MAKEFILE ON)
# path variables
set(ROOT_PROJECT_PATH "${PROJECT_SOURCE_DIR}")
set(BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
##########################################
##               options                ##
##########################################
option(BUILD_ALL "build all libraries" OFF)
option(USE_VULKAN "build circe support to vulkan" OFF)
option(BUILD_TESTS "build library unit tests" OFF)
option(BUILD_EXAMPLES "build library examples" OFF)
option(BUILD_SHARED "build shared library" OFF)
option(BUILD_DOCS "build library documentation" OFF)
option(BUILD_WITH_CUDA "build hermes with gpu enabled" OFF)
set(INSTALL_PATH ${BUILD_ROOT} CACHE STRING "include and lib folders path")
# external libs
set(HERMES_INCLUDE_PATH "" CACHE STRING "hermes include path")
set(HERMES_LIB_PATH "" CACHE STRING "hermes lib path")
# cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
# check for specific machine/compiler options.
include(ConfigCompilerFlags)
##########################################
##             dependencies             ##
##########################################
# include external libraries path
include(ExternalProject)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext")
include(catch2)
include(glfw)
include(tinyobj)
include(stb)
include(glad)
include(dirent)
include(hermes)
if (USE_VULKAN)
    include(vulkan)
endif (USE_VULKAN)


##########################################
##               source                ##
##########################################
set(IMGUI_SOURCES
        circe/imgui/imgui.cpp
        circe/imgui/imgui_draw.cpp
        circe/imgui/imgui_widgets.cpp
        circe/imgui/ImGuiFileDialog.cpp
        circe/imgui/TextEditor.cpp
        circe/imgui/ImGuizmo.cpp
        )

set(IMGUI_HEADERS
        circe/imgui/imconfig.h
        circe/imgui/imgui.h
        circe/imgui/imgui_internal.h
        circe/imgui/imstb_rectpack.h
        circe/imgui/imstb_textedit.h
        circe/imgui/imstb_truetype.h
        circe/imgui/ImGuiFileDialog.h
        circe/imgui/ImGuiFileDialogConfig.h
        circe/imgui/TextEditor.h
        circe/imgui/ImGuizmo.h
        )

set(CIRCE_HEADERS
        circe/colors/color.h
        circe/colors/color_palette.h
        circe/common/bitmask_operators.h
        #        circe/io/utils.h
        circe/scene/bvh.h
        circe/scene/array.h
        circe/scene/camera_interface.h
        circe/scene/camera_projection.h
        circe/scene/light.h
        circe/scene/material.h
        circe/scene/model.h
        circe/scene/shapes.h
        circe/scene/spatial_structure_interface.h
        circe/ui/imgui_utils.h
        circe/ui/gizmo.h
        circe/ui/trackball.h
        circe/ui/trackball_interface.h
        circe/ui/track_mode.h
        circe/ui/ui_camera.h
        circe/io/io.h
        circe/circe.h
        )

set(CIRCE_SOURCES
        #        circe/io/utils.cpp
        circe/scene/bvh.cpp
        circe/scene/model.cpp
        circe/scene/shapes.cpp
        circe/ui/imgui_utils.cpp
        circe/ui/trackball_interface.cpp
        circe/ui/ui_camera.cpp
        circe/circe.cpp
        circe/io/io.cpp
        )

set(CIRCE_GL_HEADERS
        circe/gl/scene/instance_set.h
        circe/gl/utils/open_gl.h
        circe/gl/utils/win32_utils.h
        circe/gl/utils/base_app.h
        circe/gl/scene/quad.h
        circe/gl/scene/scene.h
        circe/gl/scene/scene_mesh.h
        circe/gl/scene/scene_object.h
        circe/gl/scene/triangle_mesh.h
        #        circe/gl/scene/volume_box.h
        circe/gl/scene/wireframe_mesh.h
        circe/gl/storage/buffer_interface.h
        circe/gl/storage/device_memory.h
        circe/gl/storage/index_buffer.h
        circe/gl/storage/shader_storage_buffer.h
        circe/gl/storage/vertex_array_object.h
        circe/gl/storage/vertex_attributes.h
        circe/gl/storage/uniform_buffer.h
        circe/gl/storage/vertex_buffer.h
        circe/gl/ui/app.h
        circe/gl/ui/interactive_object_interface.h
        circe/gl/ui/modifier_cursor.h
        circe/gl/ui/picker.h
        circe/gl/ui/scene_app.h
        #        circe/gl/ui/text_renderer.h
        #        circe/gl/ui/text_object.h
        #        circe/gl/ui/font_manager.h
        circe/gl/scene/mesh_utils.h
        circe/gl/graphics/compute_shader.h
        circe/gl/graphics/ibl.h
        circe/gl/graphics/post_effect.h
        circe/gl/graphics/shader.h
        circe/gl/graphics/shader_manager.h
        circe/gl/graphics/shadow_map.h
        #        circe/gl/helpers/bbox_model.h
        #        circe/gl/helpers/segment_model.h
        #        circe/gl/helpers/camera_model.h
        circe/gl/helpers/cartesian_grid.h
        #        circe/gl/helpers/geometry_drawers.h
        #        circe/gl/helpers/grid_model.h
        #        circe/gl/helpers/quad_tree_model.h
        #        circe/gl/helpers/scene_handle.h
        #        circe/gl/helpers/vector_grid.h
        # imgui
        circe/gl/imgui/imgui_impl_glfw.h
        circe/gl/imgui/imgui_impl_opengl3.h
        circe/gl/io/buffer.h
        circe/gl/io/display_renderer.h
        circe/gl/io/framebuffer.h
        circe/gl/io/graphics_display.h
        circe/gl/texture/image_texture.h
        #        circe/gl/io/font_texture.h
        circe/gl/texture/framebuffer_texture.h
        circe/gl/io/screen_quad.h
        circe/gl/texture/texture.h
        circe/gl/io/viewport_display.h
        circe/gl/io/user_input.h
        circe/gl/scene/scene_model.h
        )

set(CIRCE_GL_SOURCES
        circe/gl/graphics/compute_shader.cpp
        circe/gl/graphics/ibl.cpp
        circe/gl/graphics/post_effect.cpp
        circe/gl/graphics/shader.cpp
        circe/gl/graphics/shader_manager.cpp
        circe/gl/graphics/shadow_map.cpp
        #        circe/gl/helpers/bbox_model.cpp
        #        circe/gl/helpers/segment_model.cpp
        circe/gl/helpers/cartesian_grid.cpp
        #        circe/gl/helpers/geometry_drawers.cpp
        circe/gl/imgui/imgui_impl_glfw.cpp
        circe/gl/imgui/imgui_impl_opengl3.cpp
        circe/gl/io/buffer.cpp
        circe/gl/io/display_renderer.cpp
        circe/gl/io/framebuffer.cpp
        circe/gl/io/graphics_display.cpp
        circe/gl/io/font_texture.cpp
        circe/gl/io/screen_quad.cpp
        circe/gl/io/viewport_display.cpp
        circe/gl/scene/instance_set.cpp
        circe/gl/scene/mesh_utils.cpp
        circe/gl/scene/quad.cpp
        circe/gl/scene/scene_mesh.cpp
        circe/gl/scene/scene_model.cpp
        circe/gl/scene/triangle_mesh.cpp
        #        circe/gl/scene/volume_box.cpp
        circe/gl/scene/wireframe_mesh.cpp
        circe/gl/storage/buffer_interface.cpp
        circe/gl/storage/device_memory.cpp
        circe/gl/storage/index_buffer.cpp
        circe/gl/storage/vertex_array_object.cpp
        circe/gl/storage/vertex_attributes.cpp
        circe/gl/storage/uniform_buffer.cpp
        circe/gl/storage/vertex_buffer.cpp
        circe/gl/storage/shader_storage_buffer.cpp
        circe/gl/texture/framebuffer_texture.cpp
        circe/gl/texture/image_texture.cpp
        circe/gl/texture/texture.cpp
        circe/gl/ui/app.cpp
        circe/gl/ui/picker.cpp
        #        circe/gl/ui/text_renderer.cpp
        #        circe/gl/ui/text_object.cpp
        #        circe/gl/ui/font_manager.cpp
        circe/gl/utils/base_app.cpp
        circe/gl/utils/open_gl.cpp
        )

set(CIRCE_VK_HEADERS)
set(CIRCE_VK_SOURCES)

if (USE_VULKAN)
    set(CIRCE_VK_HEADERS
            circe/vk/core/instance.h
            circe/vk/core/logical_device.h
            circe/vk/core/physical_device.h
            circe/vk/core/sync.h
            circe/vk/io/graphics_display.h
            circe/vk/io/surface_khr.h
            circe/vk/io/swapchain.h
            circe/vk/pipeline/command_buffer.h
            circe/vk/pipeline/pipeline.h
            circe/vk/pipeline/shader_module.h
            circe/vk/pipeline/renderpass.h
            circe/vk/scene/scene_model.h
            circe/vk/storage/buffer.h
            circe/vk/storage/device_memory.h
            circe/vk/storage/image.h
            circe/vk/texture/sampler.h
            circe/vk/texture/texture.h
            circe/vk/utils/base_app.h
            circe/vk/utils/render_engine.h
            circe/vk/utils/vk_debug.h
            circe/vk/vk_library.h
            )
    set(CIRCE_VK_SOURCES
            circe/vk/core/instance.cpp
            circe/vk/core/logical_device.cpp
            circe/vk/core/physical_device.cpp
            circe/vk/core/sync.cpp
            circe/vk/io/graphics_display.cpp
            circe/vk/io/surface_khr.cpp
            circe/vk/io/swapchain.cpp
            circe/vk/pipeline/command_buffer.cpp
            circe/vk/pipeline/pipeline.cpp
            circe/vk/pipeline/shader_module.cpp
            circe/vk/pipeline/renderpass.cpp
            circe/vk/scene/scene_model.cpp
            circe/vk/storage/buffer.cpp
            circe/vk/storage/device_memory.cpp
            circe/vk/storage/image.cpp
            circe/vk/texture/sampler.cpp
            circe/vk/texture/texture.cpp
            circe/vk/utils/base_app.cpp
            circe/vk/utils/render_engine.cpp
            )
endif (USE_VULKAN)

add_library(circe STATIC
        ${CIRCE_SOURCES} ${CIRCE_HEADERS}
        ${IMGUI_HEADERS} ${IMGUI_SOURCES}
        ${CIRCE_GL_HEADERS} ${CIRCE_GL_SOURCES}
        ${CIRCE_VK_HEADERS} ${CIRCE_VK_SOURCES}
        )

set_target_properties(circe PROPERTIES
        OUTPUT_NAME "circe"
        FOLDER "CIRCE")

target_compile_definitions(circe PUBLIC
        -DASSETS_PATH="${CIRCE_SOURCE_DIR}/examples/assets"
        -DSHADERS_PATH="${CIRCE_SOURCE_DIR}/examples/shaders"
        )

target_include_directories(circe PUBLIC
        ${CIRCE_SOURCE_DIR}
        ${STB_INCLUDES}
        ${PLY_INCLUDES}
        ${TINYOBJ_INCLUDE_DIR}
        ${GLAD_INCLUDES}
        ${GLFW_INCLUDE_DIR}
        ${DIRENT_INCLUDE_DIR}
        ${HERMES_INCLUDES}
        )
target_link_libraries(circe PUBLIC
        ${HERMES_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${VULKAN_LIBRARIES}
        ${PLY_LIBS}
        ${TINYOBJ_LIBRARIES}
        glad
        )

# if(glfw AND glad)
add_dependencies(circe hermes glfw glad tinyobj)
# endif(glfw AND glad)

##########################################
##              install                ##
##########################################
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/circe
        DESTINATION ${INSTALL_PATH}/include
        FILES_MATCHING REGEX "(.*\\.[inl|h])")

install(TARGETS circe #aergia_shared
        LIBRARY DESTINATION ${INSTALL_PATH}/lib
        ARCHIVE DESTINATION ${INSTALL_PATH}//lib
        )

##########################################
##                tests                 ##
##########################################
if (BUILD_TESTS OR BUILD_ALL)
    add_subdirectory(tests)
endif (BUILD_TESTS OR BUILD_ALL)
##########################################
##              examples                ##
##########################################
if (BUILD_EXAMPLES OR BUILD_ALL)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES OR BUILD_ALL)
#########################################
##           documentation              ##
##########################################
if (BUILD_DOCS)
    # TODO
endif (BUILD_DOCS)
##########################################
##         output variables             ##
##########################################
get_directory_property(CIRCE_HAS_PARENT PARENT_DIRECTORY)
if (CIRCE_HAS_PARENT)
    set(CIRCE_INCLUDES ${HERMES_SOURCE_DIR} ${CIRCE_SOURCE_DIR} ${GLAD_INCLUDES} PARENT_SCOPE)
    set(CIRCE_EXTRA_LIBS ${GLAD_LIBS} PARENT_SCOPE)
endif (CIRCE_HAS_PARENT)
#
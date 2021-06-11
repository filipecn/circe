/// Copyright (c) 2021, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file hello_vulkan.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-05-25
///
///\brief

#include <circe/vk/core/instance.h>
#include <circe/vk/utils/vk_debug.h>
#include <circe/vk/core/logical_device.h>
#include <circe/vk/io/graphics_display.h>
#include <circe/vk/utils/render_engine.h>

int main() {
  // Let's create a simple graphics application with vulkan.
  // A Vulkan application contains several pieces that must be configured. The
  // main parts are:
  //  1. An INSTANCE object, that connects our code to the vulkan library and
  //     is the starting point to create the remaining pieces.
  circe::vk::Instance instance;
  //  2. A PHYSICAL DEVICE, which is the actual piece of hardware we will use
  //     to run our application. The physical device must support all the
  //     features our application requires.
  circe::vk::PhysicalDevice physical_device;
  //     The chosen device must provide queues that can process the types of
  //     commands our application will dispatch. Usually we need a queue to
  //     process rendering and a queue to present the rendered images on the
  //     screen. Here we will use the QueueFamilies struct to help us storing
  //     the queues information.
  circe::vk::QueueFamilies queue_families;
  //  3. A LOGICAL DEVICE, that serves as a digital representation of the
  //     physical device. This object will be our main interface to send
  //     commands to the GPU.
  circe::vk::LogicalDevice logical_device;

  // To complete our application we will use some auxiliary classes
  //  4. The GraphicsDisplay class, that uses the GLFW to create a window
  //     context for us.
  circe::vk::GraphicsDisplay window;
  //     This class will offer the presentation surface that we
  //     will use to render and display the final image on the screen.
  circe::vk::SurfaceKHR presentation_surface;
  //  5. The RenderEngine class, that will store our rendering pipeline
  //     structures and take care of all the submission process to the
  //     physical device
  circe::vk::RenderEngine render_engine;

  // Now that we know all the parts we will use, lets initialize them in
  // the right order. Let's start with the window object:
  PONOS_ASSERT(window.init({800, 800}, "Hello Vulkan"))
  // 1. INTERFACE
  // --------------------------------------------------------------------------
  // We must start with the instance. Here we can pass a list of vulkan instance
  // extensions that our application need and validation layers to help us on
  // checking our code. The minimal set of extensions we need is the set of
  // extensions required for displaying our graphics in the window, which are
  // provided by the graphics display object
  PONOS_ASSERT(instance.init("hello_vulkan_app",
                             circe::vk::GraphicsDisplay::requiredVkExtensions(),
                             {"VK_LAYER_KHRONOS_validation"}))
  // 2. PHYSICAL DEVICE
  // --------------------------------------------------------------------------
  // The first step now is to choose a physical device that suits our needs.
  // The queue used for rendering must contain the VK_QUEUE_GRAPHICS_BIT. The
  // queue used for presentation must be able to work with the type of data the
  // presentation surface, which we get from the graphics display object:
  PONOS_ASSERT((presentation_surface = window.createWindowSurface(instance)).good())
  // By default, the method for picking a physical device will prefer integrated
  // cards:
  PONOS_ASSERT((physical_device = instance.pickPhysicalDevice(queue_families, presentation_surface.handle())).good())
  // Here is the physical device we just chose and the indices of the queues we will use
  PONOS_LOG_VARIABLE(queue_families.family("graphics").family_index.value())
  PONOS_LOG_VARIABLE(queue_families.family("presentation").family_index.value())
  // 3. DIGITAL DEVICE
  // --------------------------------------------------------------------------
  // As with the instance, the logical device also has extensions that we can
  // use and validation layers. Here we require the VK_KHR_SWAPCHAIN_EXTENSION,
  // which will allow our application to swap presentation images in order to
  // perform buffering.
  PONOS_ASSERT(logical_device.init(&physical_device, {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
                                   {}, queue_families, {"VK_LAYER_KHRONOS_validation"}))
  // 5. RENDER ENGINE
  // --------------------------------------------------------------------------
  // The render engine will work directly with a logical device and submit all
  // rendering commands to a particular family queue. So the first thing is to
  // let it know which device and queue we will use
  PONOS_ASSERT(render_engine.setupDevice(logical_device,
                                         queue_families.family("graphics").family_index.value()))
  // The other initial configuration is the presentation surface. The internal
  // settings of the rendering engine must match the desired presentation
  // image format and, of course, the presentation surface object we got
  // from the graphics display
  PONOS_ASSERT(render_engine.setPresentationSurface(presentation_surface))
  // Whenever our window is resized, all rendering structures inside the
  // render engine must be updated, so we must let the render engine
  // know when it happens
  window.resize_callback = [&](int new_w, int new_h) {
    render_engine.resize({static_cast<unsigned int>(new_w), static_cast<unsigned int>(new_h)});
  };
  return 0;
}
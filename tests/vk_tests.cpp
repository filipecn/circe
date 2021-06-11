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
///\file vk_tests.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-05-28
///
///\brief

#include <catch2/catch.hpp>

#include <circe/vk/io/graphics_display.h>
#include <circe/vk/core/instance.h>
#include <circe/vk/utils/render_engine.h>
#include <circe/vk/pipeline/renderpass.h>

using namespace circe::vk;

TEST_CASE("Raw Sanity Check") {
  // Here  is the list of all objects we need
  GraphicsDisplay window;
  Instance instance;
  SurfaceKHR surface;
  QueueFamilies queue_families;
  PhysicalDevice physical_device;
  LogicalDevice logical_device;
  RenderEngine render_engine;
  //
  {
    REQUIRE(window.init({800, 800}, "test"));
  } // "Initalize Window Object"
  {
    REQUIRE(instance.init("test",
                          circe::vk::GraphicsDisplay::requiredVkExtensions(),
                          {"VK_LAYER_KHRONOS_validation"}));
  } // "Initialize Instance Object"
  {
    surface = window.createWindowSurface(instance);
  } // "Retrieve Presentation Surface Object"
  {
    physical_device = instance.pickPhysicalDevice(queue_families, surface.handle());
    REQUIRE(physical_device.good());
    REQUIRE(logical_device.init(&physical_device, {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
                                {}, queue_families, {"VK_LAYER_KHRONOS_validation"}));
  } // "Initialize Logical Device Object"
  {
    REQUIRE(render_engine.setupDevice(logical_device,
                                      queue_families.family("graphics").family_index.value()));
    REQUIRE(render_engine.setPresentationSurface(surface));
  } // "Initialize Render Engine Object"
  // connect window size to render engine
  window.resize_callback = [&](int new_w, int new_h) {
    render_engine.resize({static_cast<unsigned int>(new_w), static_cast<unsigned int>(new_h)});
  };
}

TEST_CASE("Instance") {
  REQUIRE(Instance("test").good());
}

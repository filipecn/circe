//
// Created by filipecn on 09/06/2021.
//


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
///\file base_app.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-06-09
///
///\brief

#ifndef CIRCE_CIRCE_VK_UTILS_BASE_APP_H
#define CIRCE_CIRCE_VK_UTILS_BASE_APP_H

#include <circe/vk/io/graphics_display.h>
#include <circe/vk/core/instance.h>
#include <circe/vk/utils/render_engine.h>
#include <circe/vk/storage/device_memory.h>

namespace circe::vk {

/// Auxiliary class to quickly prototype vulkan based apps
/// This class provides a full working basic pipeline with a window
class BaseApp {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  explicit BaseApp(const ponos::size2 &window_size, const std::string &title = "base_app");
  virtual ~BaseApp();
  // ***********************************************************************
  //                           INITIALIZATION
  // ***********************************************************************
  virtual void initInstance();
  virtual void initPresentationSurface();
  virtual void initPhysicalDeviceAndFamilyQueues();
  virtual void initLogicalDevice();
  virtual void initRenderEngine();
  virtual void initRenderpass();
  virtual void init();
  // ***********************************************************************
  //                           SWAPCHAIN
  // ***********************************************************************
  virtual void createSwapchain();
  virtual void destroySwapchain();
  // ***********************************************************************
  //                           RENDERING
  // ***********************************************************************
  virtual void recordCommandBuffer(CommandBuffer &cb, u32 i) = 0;
  virtual void prepareFrameImage(uint32_t index) = 0;
  virtual void nextFrame();
  virtual void render();
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  virtual void resize(const ponos::size2 &new_window_size) = 0;
  virtual void prepare() = 0;
  int run();
protected:
  // WINDOW
  GraphicsDisplay window_;
  // CORE
  Instance instance_;
  SurfaceKHR presentation_surface_;
  QueueFamilies queue_families_;
  PhysicalDevice physical_device_;
  LogicalDevice device_;
  RenderEngine render_engine_;
  // PRESENTATION
  // 0. Renderpass
  RenderPass renderpass_;
  // 1. Anti-Aliasing
  VkSampleCountFlagBits msaa_samples_{VK_SAMPLE_COUNT_1_BIT};
  // 2. color buffer
  Image color_image_;
  Image::View color_image_view_;
  DeviceMemory color_image_memory_;
  // 3. depth buffer
  VkFormat depth_format_{};
  Image depth_image_;
  Image::View depth_image_view_;
  DeviceMemory depth_image_memory_;
  // 4. framebuffers
  std::vector<Framebuffer> framebuffers_;

  // Frame counter to display fps
  uint32_t frame_counter_ = 0;
  uint32_t last_FPS_ = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_timestamp_;
  ///  Last frame time measured using a high performance timer (if available)
  f32 frame_timer_ = 1.0f;
};

}

#endif //CIRCE_CIRCE_VK_UTILS_BASE_APP_H

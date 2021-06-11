/// Copyright (c) 2019, FilipeCN.
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
/// \file vk_render_engine.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-12-06
///
/// \brief

#ifndef CIRCE_VK_RENDER_ENGINE_H
#define CIRCE_VK_RENDER_ENGINE_H

#include <ponos/common/size.h>
#include <circe/vk/io/swapchain.h>
#include <circe/vk/io/surface_khr.h>
#include <circe/vk/pipeline/renderpass.h>
#include <circe/vk/pipeline/command_buffer.h>
#include <circe/vk/core/sync.h>

namespace circe::vk {

/// The render engine holds and controls the set of resources regarding the
/// presentation of the render image on the screen. It includes the swapchain
/// and takes care of the image submission for display.
class RenderEngine {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  RenderEngine();
  /// \param logical_device
  /// \param queue_family_index
  [[maybe_unused]] explicit RenderEngine(const LogicalDevice &logical_device,
                                         u32 queue_family_index);
  ~RenderEngine();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  // ***********************************************************************
  //                            CREATION
  // ***********************************************************************
  ///
  void destroy();
  ///
  void init();
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  /// Setups the swapchain structure -- responsible for image presentation
  /// on screen. It is configured with image format, color space and other
  /// settings. If the swap chain is successfully created, the method retrieves
  /// the list of swap chain images.
  /// \param format **[in]** desired image format
  /// \param color_space **[in]** desired color space
  /// \return bool true if success
  bool setPresentationSurface(const SurfaceKHR &surface,
                              VkFormat desired_format = VK_FORMAT_B8G8R8A8_UNORM,
                              VkColorSpaceKHR desired_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  ///
  /// \param graphics_queue
  /// \param presentation_queue
  void draw(VkQueue graphics_queue, VkQueue presentation_queue);

  // ***********************************************************************
  //                            FIELDS
  // ***********************************************************************
  /// \param resolution
  void resize(const ponos::size2 &resolution);
  /// \param logical_device
  /// \param queue_family_index
  bool setupDevice(const LogicalDevice &logical_device,
                   u32 queue_family_index);
  /// \return
  [[nodiscard]] const Swapchain &swapchain() const;
  /// \return
  [[nodiscard]] const std::vector<Image::View> &swapchainImageViews() const;
  /// \return
  std::vector<CommandBuffer> &commandBuffers();
  // ***********************************************************************
  //                           CALLBACKS
  // ***********************************************************************
  std::function<void(const ponos::size2 &)> resize_callback;
  std::function<void(CommandBuffer &, u32)> record_command_buffer_callback;
  std::function<void()> destroy_swapchain_callback;
  std::function<void()> create_swapchain_callback;
  std::function<void(u32)> prepare_frame_callback;

  ///
  void recreateSwapchain();
private:
  // ***********************************************************************
  //                          SWAPCHAIN CREATION
  // ***********************************************************************
  /// \param surface_capabilities
  /// \param number_of_images
  /// \return
  static bool selectNumberOfSwapchainImages(
      VkSurfaceCapabilitiesKHR const &surface_capabilities,
      u32 &number_of_images);
  /// \param surface_capabilities
  /// \param size_of_images
  /// \return
  static bool chooseSizeOfSwapchainImages(
      VkSurfaceCapabilitiesKHR const &surface_capabilities,
      VkExtent2D &size_of_images);
  void initSwapchain();
  ///
  void destroySwapchain();

  const PhysicalDevice *physical_device_ = nullptr;
  LogicalDevice::Ref logical_device_;

  const size_t max_frames_in_flight = 2;
  // swapchain information
  Swapchain swapchain_;
  std::vector<Image::View> swapchain_image_views_;
  // command buffers
  CommandPool draw_command_pool_; //!< command pool used for draw command buffers
  std::vector<CommandBuffer> draw_command_buffers_; //!< command buffers used for rendering
  // synchronization
  std::vector<Semaphore> render_finished_semaphores_;
  std::vector<Semaphore> image_available_semaphores_;
  std::vector<Fence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;
  // resize info
  bool framebuffer_resized_ = false;
  ponos::size2 resolution_;
};

} // namespace circe::vk

#endif
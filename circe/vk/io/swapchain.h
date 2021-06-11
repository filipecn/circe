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
///\file vk_swap_chain.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-30
///
///\brief

#ifndef CIRCE_VULKAN_SWAP_CHAIN_H
#define CIRCE_VULKAN_SWAP_CHAIN_H

#include <circe/vk/storage/image.h>
#include <circe/vk/io/surface_khr.h>

namespace circe::vk {

/// VULKAN SWAP CHAIN
// -----------------
// Different from other high level libraries, such as OpenGL, Vulkan does
// not have a system of framebuffers. In order to control the buffers that
// are rendered and presented on the display, Vulkan provides a mechanism
// called swap chain. The Vulkan swapchain is a queue of images that are
// presented to the screen in a synchronized manner, following the rules and
// properties defined on its setup. The swapchain is owned by the
// presentation engine, and not by the application. We can't create the
// images or destroy them, all the application does is to request images, do
// work and give it back to the presentation engine. In order to use the
// swapchain, the device has to support the VK_KHR_swapchain extension. The
// swapchain works following a presentation mode. The presentation mode
// defines the format of an image, the number of images (double/triple
// buffering), v-sync and etc. In other words, it defines how images are
// displayed on screen. Vulkan provides 4 presentation modes:
// 1. IMMEDIATE mode
//    The image to be presented immediately replaces the image that is being
//    displayed. Screen tearing may happen when using this mode.
// 2. FIFO mode
//    When a image is presented, it is added to the queue. Images are
//    displayed on screen in sync with blanking periods (v-sync). This mode
//    is similar to OpenGL's buffer swap.
// 3. (FIFO) RELAXED mode
//    Images are displayed with blanking periods only when are faster than
//    the refresh rate.
// 4. MAILBOX mode (triple buffering)
//    There is a queue with just one element. An image waiting in this queue
//    is displayed in sync with blanking periods. When the application
//    presents an image, the new image replaces the one waiting in the
//    queue. So the displayed image is always the most recent available.
/// \note This class uses RAII
class Swapchain final {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Swapchain();
  /// \brief Create a Swapchain object
  /// \param logical_device **[in]** logical device handle
  /// \param presentation_surface **[in]** surface to which the swap chain will
  /// present
  /// \param image_count **[in]** swapchain image count (ex: 2 = double
  /// buffering, 3 = triple buffering)
  /// Note: some devices don't support multiple buffers, the min/max number of
  /// images supported can be queried by the physical device object
  /// \param surface_format **[in]** surface format. Image format and color
  /// space of the presentable images
  /// \param image_size **[in]** image size (in pixels)
  /// \param image_usage **[in]** image usages (ex: if rendering to the image as
  /// a normal color attachment, use VK_IMAGE_USAGE_COLOR_ATTACHEMENT_BIT. If
  /// rendering direct to the image, use ..._STORAGE_BIT)
  /// \param surface_transform **[in]** surface transform
  /// \param present_mode **[in]** presentation mode
  [[maybe_unused]] Swapchain(const LogicalDevice::Ref& logical_device,
            const SurfaceKHR &presentation_surface, u32 image_count,
            VkSurfaceFormatKHR surface_format, VkExtent2D image_size,
            VkImageUsageFlags image_usage,
            VkSurfaceTransformFlagBitsKHR surface_transform,
            VkPresentModeKHR present_mode);
  Swapchain(const Swapchain &other) = delete;
  Swapchain(Swapchain &&other) noexcept;
  ~Swapchain();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Swapchain &operator=(const Swapchain &other) = delete;
  Swapchain &operator=(Swapchain &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  /// \param logical_device **[in]** logical device handle
  /// \param presentation_surface **[in]** surface to which the swap chain will
  /// present
  /// \param image_count **[in]** swapchain image count (ex: 2 = double
  /// buffering, 3 = triple buffering)
  /// Note: some devices don't support multiple buffers, the min/max number of
  /// images supported can be queried by the physical device object
  /// \param surface_format **[in]** surface format. Image format and color
  /// space of the presentable images
  /// \param image_size **[in]** image size (in pixels)
  /// \param image_usage **[in]** image usages (ex: if rendering to the image as
  /// a normal color attachment, use VK_IMAGE_USAGE_COLOR_ATTACHEMENT_BIT. If
  /// rendering direct to the image, use ..._STORAGE_BIT)
  /// \param surface_transform **[in]** surface transform
  /// \param present_mode **[in]** presentation mode
  bool init(const LogicalDevice::Ref &logical_device,
            const SurfaceKHR &presentation_surface, u32 image_count,
            VkSurfaceFormatKHR surface_format, VkExtent2D image_size,
            VkImageUsageFlags image_usage,
            VkSurfaceTransformFlagBitsKHR surface_transform,
            VkPresentModeKHR present_mode);
  /// Initializes swapchain object.
  /// If this swapchain is already an initialized swapchain, this function will
  /// destroy it first and recreate it.
  /// \return
  bool init();
  ///
  void destroy();
  /// \param presentation_surface **[in]** surface to which the swap chain will
  /// present
  /// \param image_count **[in]** swapchain image count (ex: 2 = double
  /// buffering, 3 = triple buffering)
  /// Note: some devices don't support multiple buffers, the min/max number of
  /// images supported can be queried by the physical device object
  /// \param surface_format **[in]** surface format. Image format and color
  /// space of the presentable images
  /// \param image_size **[in]** image size (in pixels)
  /// \param image_usage **[in]** image usages (ex: if rendering to the image as
  /// a normal color attachment, use VK_IMAGE_USAGE_COLOR_ATTACHEMENT_BIT. If
  /// rendering direct to the image, use ..._STORAGE_BIT)
  /// \param surface_transform **[in]** surface transform
  /// \param present_mode **[in]** presentation mode
  void set(const SurfaceKHR &presentation_surface, u32 image_count,
           VkSurfaceFormatKHR surface_format, VkExtent2D image_size,
           VkImageUsageFlags image_usage,
           VkSurfaceTransformFlagBitsKHR surface_transform,
           VkPresentModeKHR present_mode);
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  /// \return
  [[nodiscard]] bool good() const;
  /// Acquires an image index. The fence is used to make sure
  /// the application does not modify the image while there are still
  /// previously submitted operations happening on the image. The semaphore is
  /// used to tell the driver to not start processing new commands with the
  /// given image.
  /// \param semaphore **[in]** semaphore handle
  /// \param fence **[in]** fence handle
  /// \param image_index **[out]** acquired image index
  /// \return bool true if success
  VkResult nextImage(VkSemaphore semaphore, VkFence fence,
                     u32 &image_index) const;
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  /// \return
  [[nodiscard]] VkSwapchainKHR handle() const;
  const std::vector<Image::Ref> &images();
  [[nodiscard]] VkExtent2D imageSize() const;
  [[nodiscard]] VkSurfaceFormatKHR surfaceFormat() const;
  [[nodiscard]] VkSurfaceKHR surface() const;
  [[nodiscard]] VkFormat imageFormat() const;

  void setLogicalDevice(const LogicalDevice::Ref& logical_device);
  void setPresentationSurface(const SurfaceKHR& surface);
  void setNumberOfImages(u32 image_count);
  void setSurfaceFormat(VkSurfaceFormatKHR surface_format);
  void setImageSize(VkExtent2D image_extent);
  void setImageUsage(VkImageUsageFlags image_usage);
  void setSurfaceTransform(VkSurfaceTransformFlagBitsKHR surface_transform);
  void setPresentMode(VkPresentModeKHR present_mode);

private:
  VkSwapchainCreateInfoKHR info_ = {};
  LogicalDevice::Ref logical_device_;
  VkSwapchainKHR vk_swapchain_ = VK_NULL_HANDLE;
  std::vector<Image::Ref> images_{};
};

} // namespace circe

#endif // CIRCE_VULKAN_SWAP_CHAIN_H
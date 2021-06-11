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
///\file vk_swap_chain.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-30
///
///\brief

#include <circe/vk/io/swapchain.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

Swapchain::Swapchain() {
  info_.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info_.pNext = nullptr;
  info_.flags = 0;
  info_.imageArrayLayers = 1;
  info_.queueFamilyIndexCount = 0;
  info_.pQueueFamilyIndices = nullptr;
  info_.clipped = VK_TRUE;
  info_.oldSwapchain = VK_NULL_HANDLE;
  info_.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
}

Swapchain::Swapchain(const LogicalDevice::Ref &logical_device,
                     const SurfaceKHR &presentation_surface, u32 image_count,
                     VkSurfaceFormatKHR surface_format, VkExtent2D image_size,
                     VkImageUsageFlags image_usage,
                     VkSurfaceTransformFlagBitsKHR surface_transform,
                     VkPresentModeKHR present_mode) : Swapchain() {
  init(logical_device, presentation_surface, image_count, surface_format, image_size,
       image_usage, surface_transform, present_mode);
}

Swapchain::Swapchain(Swapchain &&other) noexcept {
  info_ = other.info_;
  logical_device_ = other.logical_device_;
  vk_swapchain_ = other.vk_swapchain_;
  images_ = std::move(other.images_);
  other.vk_swapchain_ = VK_NULL_HANDLE;
}

Swapchain::~Swapchain() { destroy(); }

Swapchain &Swapchain::operator=(Swapchain &&other) noexcept {
  destroy();
  info_ = other.info_;
  logical_device_ = other.logical_device_;
  vk_swapchain_ = other.vk_swapchain_;
  images_ = std::move(other.images_);
  other.vk_swapchain_ = VK_NULL_HANDLE;
  return *this;
}

bool Swapchain::init(const LogicalDevice::Ref &logical_device,
                     const SurfaceKHR &presentation_surface,
                     u32 image_count,
                     VkSurfaceFormatKHR surface_format,
                     VkExtent2D image_size,
                     VkImageUsageFlags image_usage,
                     VkSurfaceTransformFlagBitsKHR surface_transform,
                     VkPresentModeKHR present_mode) {
  logical_device_ = logical_device;
  set(presentation_surface, image_count, surface_format, image_size,
      image_usage, surface_transform, present_mode);
  return init();
}

bool Swapchain::init() {
  destroy();
  R_CHECK_VULKAN(vkCreateSwapchainKHR(logical_device_.handle(), &info_,
                                      nullptr, &vk_swapchain_), false)
  std::vector<VkImage> images;
  u32 images_count = 0;
  R_CHECK_VULKAN(vkGetSwapchainImagesKHR(logical_device_.handle(), vk_swapchain_,
                                         &images_count, nullptr), false)
  PONOS_RETURN_IF_NOT_WITH_LOG(0 != images_count, false,
                               "Could not enumerate swapchain images.")
  images.resize(images_count);
  R_CHECK_VULKAN(vkGetSwapchainImagesKHR(logical_device_.handle(), vk_swapchain_,
                                         &images_count, images.data()), false)
  images_.clear();
  for (auto &image : images)
    images_.emplace_back(logical_device_, image);
  return true;
}

void Swapchain::destroy() {
  if (vk_swapchain_ != VK_NULL_HANDLE) {
    PONOS_CHECK_EXP_WITH_LOG(logical_device_.waitIdle(), "Failed to wait logical device idle")
    vkDestroySwapchainKHR(logical_device_.handle(), vk_swapchain_, nullptr);
    vk_swapchain_ = VK_NULL_HANDLE;
  }
}

VkSwapchainKHR Swapchain::handle() const {
  return vk_swapchain_;
}

void Swapchain::set(const SurfaceKHR &presentation_surface, u32 image_count,
                    VkSurfaceFormatKHR surface_format, VkExtent2D image_size,
                    VkImageUsageFlags image_usage,
                    VkSurfaceTransformFlagBitsKHR surface_transform,
                    VkPresentModeKHR present_mode) {
  info_.surface = presentation_surface.handle();
  info_.minImageCount = image_count;
  info_.imageFormat = surface_format.format;
  info_.imageColorSpace = surface_format.colorSpace;
  info_.imageExtent = image_size;
  info_.imageUsage = image_usage;
  info_.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info_.preTransform = surface_transform;
  info_.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info_.presentMode = present_mode;
}

bool Swapchain::good() const {
  return vk_swapchain_;
}

VkResult Swapchain::nextImage(VkSemaphore semaphore, VkFence fence,
                              u32 &image_index) const {
  return vkAcquireNextImageKHR(logical_device_.handle(), vk_swapchain_,
                               2000000000, semaphore, fence, &image_index);
}
const std::vector<Image::Ref> &Swapchain::images() {
  return images_;
}

VkExtent2D Swapchain::imageSize() const { return info_.imageExtent; }

VkSurfaceFormatKHR Swapchain::surfaceFormat() const { return {info_.imageFormat, info_.imageColorSpace}; }

void Swapchain::setPresentationSurface(const SurfaceKHR &surface) {
  info_.surface = surface.handle();
}

void Swapchain::setSurfaceFormat(VkSurfaceFormatKHR surface_format) {
  info_.imageColorSpace = surface_format.colorSpace;
  info_.imageFormat = surface_format.format;
}

void Swapchain::setImageSize(VkExtent2D image_extent) {
  info_.imageExtent = image_extent;
}

void Swapchain::setImageUsage(VkImageUsageFlags image_usage) {
  info_.imageUsage = image_usage;
}

void Swapchain::setSurfaceTransform(VkSurfaceTransformFlagBitsKHR surface_transform) {
  info_.preTransform = surface_transform;
}

void Swapchain::setPresentMode(VkPresentModeKHR present_mode) {
  info_.presentMode = present_mode;
}

void Swapchain::setNumberOfImages(u32 image_count) {
  info_.minImageCount = image_count;
}

void Swapchain::setLogicalDevice(const LogicalDevice::Ref &logical_device) {
  logical_device_ = logical_device;
}

VkSurfaceKHR Swapchain::surface() const {
  return info_.surface;
}

VkFormat Swapchain::imageFormat() const {
  return info_.imageFormat;
}

} // namespace circe
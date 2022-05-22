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
///\file vk_fence.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-09
///
///\brief

#include <circe/vk/core/sync.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

Fence::Fence(const LogicalDevice::Ref &logical_device, VkFenceCreateFlags flags)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  VkFenceCreateInfo info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr,
                            flags};
  VkResult result =
      vkCreateFence(logical_device.handle(), &info, nullptr, &vk_fence_);
  CHECK_VULKAN(result)
  if (result != VK_SUCCESS)
    vk_fence_ = VK_NULL_HANDLE;
}

Fence::Fence(Fence &other)
    : logical_device_(other.logical_device_), vk_fence_(other.vk_fence_) {
  other.vk_fence_ = VK_NULL_HANDLE;
}

Fence::Fence(Fence &&other) noexcept
    : logical_device_(other.logical_device_), vk_fence_(other.vk_fence_) {
  other.vk_fence_ = VK_NULL_HANDLE;
}

Fence::~Fence() {
  if (vk_fence_ != VK_NULL_HANDLE) {
    vkDestroyFence(logical_device_.handle(), vk_fence_, nullptr);
    vk_fence_ = VK_NULL_HANDLE;
  }
}

VkFence Fence::handle() const { return vk_fence_; }

VkResult Fence::status() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_fence_, "using bad fence.")
  return vkGetFenceStatus(logical_device_.handle(), vk_fence_);
}

void Fence::wait() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_fence_, "using bad fence.")
  vkWaitForFences(logical_device_.handle(), 1, &vk_fence_, VK_TRUE,
                  UINT64_MAX);
}

void Fence::reset() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_fence_, "using bad fence.")
  vkResetFences(logical_device_.handle(), 1, &vk_fence_);
}

Semaphore::Semaphore(const LogicalDevice::Ref &logical_device,
                     VkSemaphoreCreateFlags flags)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  VkSemaphoreCreateInfo info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                nullptr, flags};
  VkResult result = vkCreateSemaphore(logical_device.handle(), &info, nullptr,
                                      &vk_semaphore_);
  CHECK_VULKAN(result)
  if (result != VK_SUCCESS)
    vk_semaphore_ = VK_NULL_HANDLE;
}

Semaphore::Semaphore(Semaphore &other)
    : logical_device_(other.logical_device_),
      vk_semaphore_(other.vk_semaphore_) {
  other.vk_semaphore_ = VK_NULL_HANDLE;
}

Semaphore::Semaphore(Semaphore &&other) noexcept
    : logical_device_(other.logical_device_),
      vk_semaphore_(other.vk_semaphore_) {
  other.vk_semaphore_ = VK_NULL_HANDLE;
}

Semaphore::~Semaphore() {
  if (logical_device_.good() && vk_semaphore_ != VK_NULL_HANDLE) {
    vkDestroySemaphore(logical_device_.handle(), vk_semaphore_, nullptr);
    vk_semaphore_ = VK_NULL_HANDLE;
  }
}

VkSemaphore Semaphore::handle() const { return vk_semaphore_; }

ImageMemoryBarrier::ImageMemoryBarrier() {
  vk_image_memory_barrier_.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
}

ImageMemoryBarrier::ImageMemoryBarrier(const Image &image,
                                       VkImageLayout old_layout,
                                       VkImageLayout new_layout) {
  HERMES_VALIDATE_EXP_WITH_WARNING(image.good(), "using bad image.")
  vk_image_memory_barrier_.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  vk_image_memory_barrier_.oldLayout = old_layout;
  vk_image_memory_barrier_.newLayout = new_layout;
  vk_image_memory_barrier_.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  vk_image_memory_barrier_.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  vk_image_memory_barrier_.image = image.handle();
  vk_image_memory_barrier_.subresourceRange.aspectMask =
      VK_IMAGE_ASPECT_COLOR_BIT;
  vk_image_memory_barrier_.subresourceRange.baseMipLevel = 0;
  vk_image_memory_barrier_.subresourceRange.levelCount = image.mipLevels();
  vk_image_memory_barrier_.subresourceRange.baseArrayLayer = 0;
  vk_image_memory_barrier_.subresourceRange.layerCount = 1;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    vk_image_memory_barrier_.srcAccessMask = 0;
    vk_image_memory_barrier_.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
      new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    vk_image_memory_barrier_.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vk_image_memory_barrier_.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  } else {
    HERMES_LOG("unsupported layout transition!");
  }
}

VkImageMemoryBarrier ImageMemoryBarrier::handle() const {
  return vk_image_memory_barrier_;
}

VkImageMemoryBarrier &ImageMemoryBarrier::handle() {
  return vk_image_memory_barrier_;
}

} // namespace circe::vk
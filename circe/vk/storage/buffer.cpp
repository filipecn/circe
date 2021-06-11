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
/// \file vk_buffer.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-09-04
///
/// \brief

#include <circe/vk/storage/buffer.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

Buffer::Ref::Ref(VkBuffer vk_buffer, const LogicalDevice::Ref &logical_device) : vk_buffer_{vk_buffer},
                                                                                 logical_device_{logical_device} {
}

Buffer::Ref::~Ref() = default;

VkBuffer Buffer::Ref::handle() const { return vk_buffer_; }

const LogicalDevice::Ref &Buffer::Ref::device() const { return logical_device_; }

bool Buffer::Ref::good() const { return vk_buffer_ != VK_NULL_HANDLE; }

Buffer::Buffer(const LogicalDevice::Ref &logical_device, VkDeviceSize size,
               VkBufferUsageFlags usage, VkSharingMode sharing_mode)
    : logical_device_(logical_device) {
  if (!init(logical_device_, size, usage, sharing_mode))
    PONOS_LOG_ERROR("Could not create buffer.")
}

Buffer::Buffer(Buffer &&other) noexcept
    : logical_device_(other.logical_device_), info_(other.info_),
      vk_buffer_(other.vk_buffer_) {
  other.vk_buffer_ = VK_NULL_HANDLE;
}

Buffer::~Buffer() { destroy(); }

VkBuffer Buffer::handle() const { return vk_buffer_; }

bool Buffer::good() const { return vk_buffer_ != VK_NULL_HANDLE; }

void Buffer::destroy() {
  if (logical_device_.good() && VK_NULL_HANDLE != vk_buffer_)
    vkDestroyBuffer(logical_device_.handle(), vk_buffer_, nullptr);
}

bool Buffer::init(const LogicalDevice::Ref &logical_device, VkDeviceSize size,
                  VkBufferUsageFlags usage, VkSharingMode sharingMode) {
  destroy();
  logical_device_ = logical_device;
  info_.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info_.pNext = nullptr;
  info_.flags = 0;
  info_.size = size;
  info_.usage = usage;
  info_.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info_.queueFamilyIndexCount = 0;
  info_.pQueueFamilyIndices = nullptr;
  R_CHECK_VULKAN(
      vkCreateBuffer(logical_device.handle(), &info_, nullptr, &vk_buffer_), false)
  return true;
}

const LogicalDevice::Ref &Buffer::device() const { return logical_device_; }

VkDeviceSize Buffer::size() const { return info_.size; }

void Buffer::resize(VkDeviceSize new_size) {
  destroy();
  init(logical_device_, new_size, info_.usage, info_.sharingMode);
}

bool Buffer::memoryRequirements(
    VkMemoryRequirements &memory_requirements) const {
  vkGetBufferMemoryRequirements(logical_device_.handle(), vk_buffer_,
                                &memory_requirements);
  return true;
}

void Buffer::setDevice(const LogicalDevice::Ref &logical_device) {
  logical_device_ = logical_device;
}

Buffer::View Buffer::view(VkFormat format, VkDeviceSize memory_offset, VkDeviceSize memory_range) {
  return Buffer::View(vk_buffer_, logical_device_.handle(), format, memory_offset, memory_range);
}

Buffer::View::View(VkBuffer vk_buffer, VkDevice vk_logical_device, VkFormat format,
                   VkDeviceSize memory_offset, VkDeviceSize memory_range)
    : vk_logical_device_{vk_logical_device} {
  VkBufferViewCreateInfo buffer_view_create_info = {
      VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO, // VkStructureType sType
      nullptr,         // const void               * pNext
      0,               // VkBufferViewCreateFlags    flags
      vk_buffer, // VkBuffer                   buffer
      format,          // VkFormat                   format
      memory_offset,   // VkDeviceSize               offset
      memory_range     // VkDeviceSize               range
  };

  CHECK_VULKAN(vkCreateBufferView(vk_logical_device_,
                                  &buffer_view_create_info, nullptr,
                                  &vk_buffer_view_))
  if (vk_buffer_view_ == VK_NULL_HANDLE)
    PONOS_LOG_ERROR("Could not create buffer view.")
}

Buffer::View::View(View &&other) noexcept: vk_logical_device_{other.vk_logical_device_},
                                           vk_buffer_view_{other.vk_buffer_view_} {
  other.vk_buffer_view_ = VK_NULL_HANDLE;
}

Buffer::View::~View() {
  if (VK_NULL_HANDLE != vk_buffer_view_)
    vkDestroyBufferView(vk_logical_device_, vk_buffer_view_, nullptr);
}

bool Buffer::View::good() const { return vk_buffer_view_ != VK_NULL_HANDLE; }

} // namespace circe
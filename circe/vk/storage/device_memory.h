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
///\file vk_device_memory.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-24
///
///\brief

#ifndef CIRCE_VULKAN_DEVICE_MEMORY_H
#define CIRCE_VULKAN_DEVICE_MEMORY_H

#include <circe/vk/storage/buffer.h>
#include <circe/vk/storage/image.h>

namespace circe::vk {

class DeviceMemoryPool {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  DeviceMemoryPool(const LogicalDevice::Ref &device, VkDeviceSize size,
                   uint32_t type);
  ~DeviceMemoryPool();

private:
  LogicalDevice::Ref device_;
  VkDeviceMemory vk_device_memory_ = VK_NULL_HANDLE;
};

class DeviceMemory final {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  DeviceMemory() = default;
  explicit DeviceMemory(const Buffer &buffer,
                        VkMemoryPropertyFlags required_flags,
                        VkMemoryPropertyFlags preferred_flags = 0);
  explicit DeviceMemory(const Image &image,
                        VkMemoryPropertyFlags required_flags,
                        VkMemoryPropertyFlags preferred_flags = 0);
  DeviceMemory(const DeviceMemory &other) = delete;
  DeviceMemory(DeviceMemory &&other) noexcept;
  ~DeviceMemory();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  DeviceMemory &operator=(const DeviceMemory &other) = delete;
  DeviceMemory &operator=(DeviceMemory &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  void destroy();
  [[nodiscard]] bool good() const;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  ///\param memory_requirements **[in]**
  ///\param required_flags **[in]**
  ///\param preferred_flags **[in]**
  ///\return bool
  bool allocate(VkMemoryRequirements memory_requirements,
                VkMemoryPropertyFlags required_flags,
                VkMemoryPropertyFlags preferred_flags = 0);
  /// Attach the allocated memory block to the buffer
  ///\param buffer **[in]**
  ///\param offset **[in]**
  ///\return bool
  bool bind(const Buffer &buffer, VkDeviceSize offset = 0);
  /// Attach the allocated memory block to the image
  ///\param image **[in]**
  ///\param offset **[in]**
  ///\return bool
  bool bind(const Image &image, VkDeviceSize offset = 0);
  /// Unfortunately the driver may not immediately copy the data into the buffer
  /// memory, for example because of caching. It is also possible that writes to
  /// the buffer are not visible in the mapped memory yet. There are two ways to
  /// deal with that problem:
  /// - Use a memory heap that is host coherent, indicated with
  /// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  /// - Call vkFlushMappedMemoryRanges to after writing to the mapped memory,
  /// and call vkInvalidateMappedMemoryRanges before reading from the mapped
  /// memory
  bool copy(const void *data, VkDeviceSize size, VkDeviceSize offset = 0);
  bool map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void *mapped();
  void unmap();
  /// Flush a memory range to make it visible to the device. (Required for
  /// non-coherent memory)
  ///\param size **[in]** Size of the memory range to flush
  ///\param offset **[in]** Byte offset from beginning
  ///\return bool **true** if success
  bool flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  /// Invalidate a memory range to make it visible to the host (Required for
  /// non-coherent memory)
  ///\param size **[in]** Size of memory range to invalidate
  ///\param offset **[in]** Byte offset from beginning
  ///\return bool **true** on success
  bool invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] const LogicalDevice::Ref &device() const;
  void setDevice(const LogicalDevice::Ref &logical_device);

private:
  LogicalDevice::Ref device_;
  VkDeviceMemory vk_device_memory_ = VK_NULL_HANDLE;
  void *mapped_ = nullptr;
};

} // namespace circe

#endif
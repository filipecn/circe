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
///\file vk_fence.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-09
///
///\brief

#ifndef CIRCE_VULKAN_SYNC_H
#define CIRCE_VULKAN_SYNC_H

#include <circe/vk/storage/image.h>

namespace circe::vk {

// SYNCHRONIZATION
// ---------------
// A very important task in vulkan applications is the submission of
// operations to the hardware. The operations are submitted in form of
// commands that are stored in buffers and sent to family queues provided
// by the device. Each of these queues are specialized in certain types of
// commands and different queues can be processed simultaniously. Depending
// on the application and the commands being executed and the operations
// waiting to be executed, some dependencies might appear. One queue might
// need the operations of another queue to finish first and then complete
// its work for example. The same may happen on the application side,
// waiting for the queue to finish its work. For that, Vulkan provides
// semaphores and fences.
// - Semaphores allow us to coordinate operations submitted within one queue
//   and between different queues in one logical device. They are submitted
//   to command buffer submissions and have their state changed as soon as
//   all commands are finished. We can also specify that certain commands
//   should wait until all semaphores from a certain list get activated.
// - Fences inform the application that a submitted work is finished. A
// fence changes its state as soon all work submitted along with it is
// finished.
class Fence {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  explicit Fence(const LogicalDevice::Ref &logical_device, VkFenceCreateFlags flags = 0);
  Fence(const Fence &other) = delete;
  Fence(Fence &other);
  Fence(Fence &&other) noexcept;
  ~Fence();
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  [[nodiscard]] VkResult status() const;
  void wait() const;
  void reset() const;
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] VkFence handle() const;

private:
  LogicalDevice::Ref logical_device_;
  VkFence vk_fence_ = VK_NULL_HANDLE;
};

/// Semaphores cannot be explicitly signaled or waited by the device. Rather,
/// they are signaled by queues.
class Semaphore {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  explicit Semaphore(const LogicalDevice::Ref &logical_device,
                     VkSemaphoreCreateFlags flags = 0);
  Semaphore(const Semaphore &other) = delete;
  Semaphore(Semaphore &other);
  Semaphore(Semaphore &&other) noexcept;
  ~Semaphore();
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] VkSemaphore handle() const;

private:
  LogicalDevice::Ref logical_device_;
  VkSemaphore vk_semaphore_ = VK_NULL_HANDLE;
};

class ImageMemoryBarrier {
public:
  ImageMemoryBarrier();
  ImageMemoryBarrier(const Image &image, VkImageLayout old_layout,
                     VkImageLayout new_layout);
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] VkImageMemoryBarrier handle() const;
  [[nodiscard]] VkImageMemoryBarrier &handle();

private:
  VkImageMemoryBarrier vk_image_memory_barrier_{};
};

} // namespace circe::vk

#endif
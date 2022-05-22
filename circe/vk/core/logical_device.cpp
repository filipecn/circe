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
///\file vulkan_logical_device.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-18
///
///\brief

#include <circe/vk/core/logical_device.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

LogicalDevice::Ref::Ref(const LogicalDevice *device) : device_{device} {}

VkDevice LogicalDevice::Ref::handle() const {
  return device_->vk_device_;
}

bool LogicalDevice::Ref::good() const { return device_ && device_->vk_device_ != VK_NULL_HANDLE; }

bool LogicalDevice::Ref::waitIdle() const {
  R_CHECK_VULKAN(vkDeviceWaitIdle(device_->vk_device_), false)
  return true;
}

u32 LogicalDevice::Ref::chooseMemoryType(const VkMemoryRequirements &memory_requirements,
                                         VkMemoryPropertyFlags required_flags,
                                         VkMemoryPropertyFlags preferred_flags) const {
  return device_->physical_device_->chooseMemoryType(memory_requirements, required_flags,
                                                     preferred_flags);
}

const PhysicalDevice &LogicalDevice::Ref::physicalDevice() const {
  return *device_->physical_device_;
}

LogicalDevice::LogicalDevice() = default;

[[maybe_unused]] LogicalDevice::LogicalDevice(
    const PhysicalDevice *physical_device,
    std::vector<char const *> const &desired_extensions,
    const VkPhysicalDeviceFeatures &desired_features, QueueFamilies &queue_info,
    const std::vector<const char *> &validation_layers)
    : physical_device_(physical_device) {
  init(physical_device, desired_extensions, desired_features, queue_info, validation_layers);
}

LogicalDevice::LogicalDevice(LogicalDevice &&other) noexcept {
  vk_device_ = other.vk_device_;
  physical_device_ = other.physical_device_;
  other.vk_device_ = VK_NULL_HANDLE;
}

LogicalDevice::~LogicalDevice() {
  destroy();
}

LogicalDevice &LogicalDevice::operator=(LogicalDevice &&other) noexcept {
  destroy();
  vk_device_ = other.vk_device_;
  physical_device_ = other.physical_device_;
  other.vk_device_ = VK_NULL_HANDLE;
  return *this;
}

bool LogicalDevice::init(const PhysicalDevice *physical_device,
                         const std::vector<const char *> &desired_extensions,
                         const VkPhysicalDeviceFeatures &desired_features,
                         QueueFamilies &queue_info,
                         const std::vector<const char *> &validation_layers) {
  destroy();
  physical_device_ = physical_device;

  for (auto &extension : desired_extensions)
    if (!physical_device->isExtensionSupported(extension)) {
      HERMES_LOG_WARNING(hermes::Str::concat("Extension named '", extension,
                                             "' is not supported by a physical device.").c_str());
      return false;
    }
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  for (auto &info : queue_info.families()) {
    queue_create_infos.push_back({
                                     VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // VkStructureType sType
                                     nullptr,                   // const void                     * pNext
                                     0,                         // VkDeviceQueueCreateFlags         flags
                                     info.family_index.value(), // u32 queueFamilyIndex
                                     static_cast<u32>(info.priorities.size()), // u32 queueCount
                                     info.priorities.data() // const float  * pQueuePriorities
                                 });
  }
  VkDeviceCreateInfo device_create_info = {
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, // VkStructureType sType
      nullptr, // const void                     * pNext
      0,       // VkDeviceCreateFlags              flags
      static_cast<u32>(
          queue_create_infos.size()), // u32 queueCreateInfoCount
      queue_create_infos
          .data(), // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
      static_cast<u32>(
          validation_layers.size()), // u32 enabledLayerCount
      (validation_layers.size())
      ? validation_layers.data()
      : nullptr, // const char * const             * ppEnabledLayerNames
      static_cast<u32>(
          desired_extensions.size()), // u32 enabledExtensionCount
      desired_extensions
          .data(), // const char * const             * ppEnabledExtensionNames
      &desired_features // const VkPhysicalDeviceFeatures * pEnabledFeatures
  };
  CHECK_VULKAN(vkCreateDevice(physical_device->handle(), &device_create_info,
                              nullptr, &vk_device_));
  if (vk_device_ == VK_NULL_HANDLE)
  HERMES_LOG_WARNING("Could not create logical device.");

  for (auto &info : queue_info.families()) {
    for (size_t i = 0; i < info.priorities.size(); ++i) {
      vkGetDeviceQueue(vk_device_, info.family_index.value(), i,
                       &info.vk_queues[i]);
      if (info.vk_queues[i] == VK_NULL_HANDLE)
      HERMES_LOG_WARNING("Could not get device queue");
    }
  }
  return good();
}

void LogicalDevice::destroy() {
  if (vk_device_)
    vkDestroyDevice(vk_device_, nullptr);
  vk_device_ = VK_NULL_HANDLE;
}

LogicalDevice::Ref LogicalDevice::ref() const {
  return LogicalDevice::Ref(this);
}

VkDevice LogicalDevice::handle() const { return vk_device_; }

const PhysicalDevice *LogicalDevice::physicalDevice() const {
  return physical_device_;
}

bool LogicalDevice::good() const { return vk_device_ != VK_NULL_HANDLE; }

u32
LogicalDevice::chooseMemoryType(const VkMemoryRequirements &memory_requirements,
                                VkMemoryPropertyFlags required_flags,
                                VkMemoryPropertyFlags preferred_flags) const {
  return physical_device_->chooseMemoryType(memory_requirements, required_flags,
                                            preferred_flags);
}

bool LogicalDevice::waitIdle() const {
  R_CHECK_VULKAN(vkDeviceWaitIdle(vk_device_), false)
  return true;
}

} // namespace circe
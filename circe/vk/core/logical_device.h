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
///\file vulkan_logical_device.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-18
///
///\brief

#ifndef CIRCE_VULKAN_LOGICAL_DEVICE_H
#define CIRCE_VULKAN_LOGICAL_DEVICE_H

#include <circe/vk/core/physical_device.h>
#include <map>

namespace circe::vk {
/// Stores information about queues requested to a logical device and the list
/// of priorities assigned to each one of them
struct QueueFamilyInfo {
  std::string name;
  std::optional<u32> family_index; //!< queue family index
  std::vector<float> priorities;        //!< list of queue priorities, [0.0,1.0]
  std::vector<VkQueue> vk_queues;       //!< queue handles
};
/// Holds family queue information to be used on logical device creation and
/// command buffer submissions
/// \note Every Vulkan operation requires commands that are submitted to a queue.
/// Different queues can be processed independently and may support different
/// types of operations.
/// Queues with the same capabilities are grouped into families. A device may
/// expose any number of queue families. For example, there could be a queue
/// family that only allows processing of compute commands or one that only
/// allows memory transfer related commands.
struct QueueFamilies {
  /// \param family_index
  /// \param name family name identifier
  /// \param priorities
  void add(u32 family_index, const std::string &name,
           const std::vector<float> &priorities = {1.f}) {
    for (size_t i = 0; i < families_.size(); ++i)
      if (families_[i].family_index == family_index) {
        family_info_indices_[name] = i;
        for (auto p : priorities) {
          families_[i].priorities.emplace_back(p);
          families_[i].vk_queues.push_back(VK_NULL_HANDLE);
        }
        return;
      }
    size_t family_info_index = families_.size();
    family_info_indices_[name] = family_info_index;
    families_.emplace_back();
    families_[family_info_index].family_index = family_index;
    families_[family_info_index].priorities = priorities;
    families_[family_info_index].name = name;
    families_[family_info_index].vk_queues.resize(priorities.size(),
                                                  VK_NULL_HANDLE);
  }
  /// \return
  [[nodiscard]] const std::vector<QueueFamilyInfo> &families() const { return families_; }
  /// \return
  std::vector<QueueFamilyInfo> &families() { return families_; }
  /// Search family by its name identifier
  /// \param name
  /// \return
  const QueueFamilyInfo &family(const std::string &name) {
    auto it = family_info_indices_.find(name);
    if (it != family_info_indices_.end())
      return families_[family_info_indices_[name]];
    return families_[0];
  }

private:
  std::map<std::string, size_t> family_info_indices_;
  std::vector<QueueFamilyInfo> families_;
};
/// The logical device makes the interface of the application and the physical
/// device. Represents the hardware, along with the extensions and features
/// enabled for it and all the queues requested from it. The logical device
/// allows us to record commands, submit them to queues and acquire the
/// results.
/// Device queues need to be requested on device creation and we cannot create or
/// destroy queues explicitly. They are created/destroyed on logical device
/// creation/destruction.
/// \note This class uses RAII
class LogicalDevice {
public:
  /// Holds a reference to the logical device vulkan handle
  class Ref {
    friend class LogicalDevice;
  public:
    Ref() = default;
    [[nodiscard]] VkDevice handle() const;
    [[nodiscard]] bool good() const;
    [[nodiscard]] bool waitIdle() const;
    [[nodiscard]] u32 chooseMemoryType(const VkMemoryRequirements &memory_requirements,
                                       VkMemoryPropertyFlags required_flags,
                                       VkMemoryPropertyFlags preferred_flags) const;
    const PhysicalDevice& physicalDevice() const;

  private:
    explicit Ref(const LogicalDevice *device);
    const LogicalDevice *device_{nullptr};

  };
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  LogicalDevice();
  ///\brief Construct a new Logical Device object
  ///\param physical_device **[in]** physical device object
  /// \param desired_extensions **[in]** desired extensions
  /// \param desired_features **[in]** desired features
  /// \param queue_info **[in]** queues description
  [[maybe_unused]] LogicalDevice(const PhysicalDevice *physical_device,
                                 std::vector<char const *> const &desired_extensions,
                                 const VkPhysicalDeviceFeatures &desired_features,
                                 QueueFamilies &queue_info,
                                 const std::vector<const char *> &validation_layers =
                                 std::vector<const char *>());
  /// \param other
  LogicalDevice(const LogicalDevice &other) = delete;
  /// \param other
  LogicalDevice(LogicalDevice &&other) noexcept;
  ///\brief Default destructor
  ~LogicalDevice();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  LogicalDevice &operator=(const LogicalDevice &other) = delete;
  LogicalDevice &operator=(LogicalDevice &&other) noexcept;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  ///\brief Construct a new Logical Device object
  ///\param physical_device **[in]** physical device object
  /// \param desired_extensions **[in]** desired extensions
  /// \param desired_features **[in]** desired features
  /// \param queue_info **[in]** queues description
  bool init(const PhysicalDevice *physical_device,
            std::vector<char const *> const &desired_extensions,
            const VkPhysicalDeviceFeatures &desired_features,
            QueueFamilies &queue_info,
            const std::vector<const char *> &validation_layers =
            std::vector<const char *>());
  ///
  void destroy();
  [[nodiscard]] Ref ref() const;
  ///\brief
  ///\return VkDevice
  [[nodiscard]] VkDevice handle() const;
  ///\return const PhysicalDevice&
  [[nodiscard]] const PhysicalDevice *physicalDevice() const;
  /// Checks physical device object construction success
  ///\return bool true if this can be used
  [[nodiscard]] bool good() const;
  ///\brief Selects the index of memory type trying to satisfy the preferred
  /// requirements.
  ///\param memory_requirements **[in]** memory requirements for a particular
  /// resource.
  ///\param required_flags **[in]** hard requirements
  ///\param preferred_flags **[in]** soft requirements
  ///\return u32 memory type
  [[nodiscard]] u32 chooseMemoryType(const VkMemoryRequirements &memory_requirements,
                                     VkMemoryPropertyFlags required_flags,
                                     VkMemoryPropertyFlags preferred_flags) const;
  [[nodiscard]] bool waitIdle() const;

private:
  const PhysicalDevice *physical_device_{nullptr};
  VkDevice vk_device_{VK_NULL_HANDLE};
};

} // namespace circe

#endif
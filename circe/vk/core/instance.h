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
///\file vulkan_instance.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-16
///
///\brief

#ifndef CIRCE_VULKAN_INSTANCE_H
#define CIRCE_VULKAN_INSTANCE_H

#include <circe/vk/core/physical_device.h>
#include <circe/vk/core/logical_device.h>
#include <string>
#include <vector>
#include <functional>


namespace circe::vk {

class SupportInfo {
public:
  static bool isValidationLayerSupported(const char *validation_layer);
  /// Checks if extension is supported by the instance
  ///\param desired_instance_extension **[in]** extension name (ex: )
  ///\return bool true if extension is supported
  static bool
  isInstanceExtensionSupported(const char *desired_instance_extension);

private:
  /// Gets the list of the properties of supported instance extensions on the
  /// current hardware platform.
  /// \param extensions **[out]** list of extensions
  /// \return bool true if success
  static bool checkAvailableExtensions(std::vector<VkExtensionProperties> &extensions);
  static bool checkAvailableValidationLayers(
      std::vector<VkLayerProperties> &validation_layers);

  static std::vector<VkExtensionProperties> vk_extensions_;
  static std::vector<VkLayerProperties> vk_validation_layers_;
};

///\brief Vulkan Instance object handle
/// The Vulkan Instance holds all kinds of information about the application,
/// such as application name, version, etc. The instance is the interface
/// between the application and the Vulkan Library.
/// \note This class uses RAII
class Instance final {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Instance();
  ///\brief Construct a new Instance object
  ///\param application_name **[in]**
  ///\param desired_instance_extensions **[in | default = {}]**
  explicit Instance(const std::string &application_name,
                    const std::vector<const char *> &desired_instance_extensions =
                    std::vector<const char *>(),
                    const std::vector<const char *> &validation_layers =
                    std::vector<const char *>());
  ///
  /// \param other
  Instance(Instance &&other) noexcept;
  Instance(const Instance &other) = delete;
  ~Instance();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Instance &operator=(const Instance &other) = delete;
  Instance &operator=(Instance &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  /// Creates the instance object
  /// \note Destroys first if necessary
  /// \param application_name
  /// \param desired_instance_extensions
  /// \param validation_layers
  /// \return bool if good()
  bool init(const std::string &application_name,
            const std::vector<const char *> &desired_instance_extensions =
            std::vector<const char *>(),
            const std::vector<const char *> &validation_layers =
            std::vector<const char *>());
  ///
  void destroy();
  ///\return bool true if instance object construction succeeded
  [[nodiscard]] bool good() const;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  ///\note Uses vkEnumeratePhysicalDevices
  ///\param physical_devices **[out]** receives list of physical devices
  ///\return bool true if success
  [[nodiscard]] std::vector<PhysicalDevice> enumerateAvailablePhysicalDevices() const;
  ///
  /// \param queue_families
  /// \param desired_capabilities
  /// \param vk_surface
  /// \param score_function
  /// \return
  [[maybe_unused]] PhysicalDevice pickPhysicalDevice(QueueFamilies &queue_families,
                                                     VkSurfaceKHR vk_surface,
                                                     VkQueueFlagBits desired_capabilities = VK_QUEUE_GRAPHICS_BIT,
                                                     const std::function<u32(const PhysicalDevice &)> &score_function = [](
                                                         const PhysicalDevice &device) -> u32 {
                                                       return device.properties().deviceType
                                                                  == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 1;
                                                     }) const;
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  /// \return vulkan handle
  [[nodiscard]] VkInstance handle() const;

private:
  VkInstance vk_instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT vk_debug_messenger_ = VK_NULL_HANDLE;
};

} // namespace circe

#endif

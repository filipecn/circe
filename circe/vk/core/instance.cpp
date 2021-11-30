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
///\file vulkan_instance.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-16
///
///\brief

#include <circe/vk/core/instance.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &create_info) {
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debugCallback;
}

std::vector<VkExtensionProperties> SupportInfo::vk_extensions_;
std::vector<VkLayerProperties> SupportInfo::vk_validation_layers_;

bool SupportInfo::checkAvailableExtensions(
    std::vector<VkExtensionProperties> &extensions) {
  u32 extensions_count = 0;
  R_CHECK_VULKAN(vkEnumerateInstanceExtensionProperties(
      nullptr, &extensions_count, nullptr), false)
  HERMES_ASSERT(extensions_count != 0)
  extensions.resize(extensions_count);
  R_CHECK_VULKAN(vkEnumerateInstanceExtensionProperties(
      nullptr, &extensions_count, &extensions[0]), false)
  HERMES_ASSERT(extensions_count != 0)
  return true;
}

bool SupportInfo::checkAvailableValidationLayers(
    std::vector<VkLayerProperties> &validation_layers) {
  u32 layer_count = 0;
  R_CHECK_VULKAN(vkEnumerateInstanceLayerProperties(&layer_count, nullptr), false)
  HERMES_ASSERT(layer_count != 0)
  validation_layers.resize(layer_count);
  R_CHECK_VULKAN(vkEnumerateInstanceLayerProperties(&layer_count,
                                                    validation_layers.data()), false)
  HERMES_ASSERT(layer_count != 0)
  return true;
}

bool SupportInfo::isInstanceExtensionSupported(
    const char *desired_instance_extension) {
  static bool available_loaded = false;
  if (!available_loaded) {
    HERMES_ASSERT(checkAvailableExtensions(vk_extensions_))
    available_loaded = true;
  }

  for (const auto &extension : vk_extensions_)
    if (std::string(extension.extensionName) ==
        std::string(desired_instance_extension))
      return true;
  return false;
}

bool SupportInfo::isValidationLayerSupported(
    const char *validation_layer) {
  static bool available_loaded = false;
  if (!available_loaded) {
    HERMES_ASSERT(checkAvailableValidationLayers(vk_validation_layers_))
    available_loaded = true;
  }

  for (const auto &layer : vk_validation_layers_)
    if (std::string(layer.layerName) == std::string(validation_layer))
      return true;
  return false;
}

Instance::Instance() = default;

Instance::Instance(const std::string &application_name,
                   const std::vector<const char *> &desired_instance_extensions,
                   const std::vector<const char *> &validation_layers) {
  init(application_name, desired_instance_extensions, validation_layers);
}

Instance::Instance(Instance &&other) noexcept {
  destroy();
  vk_instance_ = other.vk_instance_;
  vk_debug_messenger_ = other.vk_debug_messenger_;
  other.vk_instance_ = VK_NULL_HANDLE;
  other.vk_debug_messenger_ = VK_NULL_HANDLE;
}

Instance::~Instance() {
  destroy();
}

Instance &Instance::operator=(Instance &&other) noexcept {
  destroy();
  vk_instance_ = other.vk_instance_;
  vk_debug_messenger_ = other.vk_debug_messenger_;
  other.vk_instance_ = VK_NULL_HANDLE;
  other.vk_debug_messenger_ = VK_NULL_HANDLE;
  return *this;
}

bool Instance::init(const std::string &application_name,
                    const std::vector<const char *> &desired_instance_extensions,
                    const std::vector<const char *> &validation_layers) {
  destroy();
  SupportInfo support_info;
  std::vector<const char *> instance_extensions = desired_instance_extensions;
  instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  for (auto &extension : instance_extensions)
    if (!support_info.isInstanceExtensionSupported(extension)) {
      HERMES_LOG_WARNING(concat("Extension named '", extension, "' is not supported."))
      return false;
    }
  for (auto &layer : validation_layers)
    if (!support_info.isValidationLayerSupported(layer)) {
      HERMES_LOG_WARNING(concat("Validation layer named '", layer, "' is not supported."))
      return false;
    }
  VkApplicationInfo info;
  info.pApplicationName = application_name.c_str();
  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
  info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  info.pNext = nullptr;
  info.pEngineName = "circe";
  VkInstanceCreateInfo create_info;
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = nullptr;
  create_info.flags = 0;
  create_info.pApplicationInfo = &info;
  create_info.enabledLayerCount = validation_layers.size();
  create_info.ppEnabledLayerNames =
      (validation_layers.size()) ? validation_layers.data() : nullptr;
  create_info.enabledExtensionCount =
      static_cast<u32>(instance_extensions.size());
  create_info.ppEnabledExtensionNames =
      (instance_extensions.size()) ? instance_extensions.data() : nullptr;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  populateDebugMessengerCreateInfo(debug_create_info);
  create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debug_create_info;

  R_CHECK_VULKAN(vkCreateInstance(&create_info, nullptr, &vk_instance_), false)

  VkDebugUtilsMessengerCreateInfoEXT debug_after_create_info;
  populateDebugMessengerCreateInfo(debug_after_create_info);
  R_CHECK_VULKAN(CreateDebugUtilsMessengerEXT(vk_instance_, &debug_after_create_info,
                                              nullptr, &vk_debug_messenger_), false)
  return good();
}

void Instance::destroy() {
  if (vk_instance_) {
    if (vk_debug_messenger_)
      DestroyDebugUtilsMessengerEXT(vk_instance_, vk_debug_messenger_, nullptr);
    vkDestroyInstance(vk_instance_, nullptr);
  }
  vk_debug_messenger_ = VK_NULL_HANDLE;
  vk_instance_ = VK_NULL_HANDLE;
}

VkInstance Instance::handle() const { return vk_instance_; }

bool Instance::good() const { return vk_instance_ != VK_NULL_HANDLE; }

std::vector<PhysicalDevice> Instance::enumerateAvailablePhysicalDevices() const {
  std::vector<PhysicalDevice> physical_devices;
  u32 devices_count = 0;
  R_CHECK_VULKAN(
      vkEnumeratePhysicalDevices(vk_instance_, &devices_count, nullptr), physical_devices)
  HERMES_LOG_AND_RETURN_VALUE_IF_NOT(
      devices_count != 0, physical_devices,
      "Could not get the number of available physical devices.")
  std::vector<VkPhysicalDevice> devices(devices_count);
  R_CHECK_VULKAN(
      vkEnumeratePhysicalDevices(vk_instance_, &devices_count, devices.data()), physical_devices)
  HERMES_LOG_AND_RETURN_VALUE_IF_NOT(devices_count != 0, physical_devices,
                               "Could not enumerate physical devices.")
  for (auto &device : devices)
    physical_devices.emplace_back(device);

  return physical_devices;
}

PhysicalDevice Instance::pickPhysicalDevice(QueueFamilies &queue_families,
                                            VkSurfaceKHR vk_surface,
                                            VkQueueFlagBits desired_capabilities,
                                            const std::function<u32(const PhysicalDevice &)> &score_function) const {
  auto physical_devices = enumerateAvailablePhysicalDevices();
  // ordered map of <score, device index>
  std::multimap<u32, u32> candidates;
  std::vector<QueueFamilies> queue_families_list(physical_devices.size());
  for (u32 i = 0; i < physical_devices.size(); ++i) {
    u32 presentation_family = 0;
    u32 graphics_family = 0;
    // find a family that supports capabilities and, if required, surface presentation
    if (physical_devices[i].selectIndexOfQueueFamily(desired_capabilities,
                                                     graphics_family)) {
      if (vk_surface && !physical_devices[i].selectIndexOfQueueFamily(vk_surface,
                                                                      presentation_family))
        continue;
      queue_families_list[i].add(graphics_family, "graphics");
      if (vk_surface)
        queue_families_list[i].add(presentation_family, "presentation");
      candidates.insert(std::make_pair(score_function(physical_devices[i]), i));
    }
  }
  if (!candidates.empty() && candidates.rbegin()->first > 0) {
    queue_families = queue_families_list[candidates.rbegin()->second];
    return physical_devices[candidates.rbegin()->second];
  }
  HERMES_LOG_WARNING("failed to find a suitable physical device!")
  return PhysicalDevice();
}

} // namespace circe
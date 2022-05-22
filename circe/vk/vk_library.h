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
/// \file vulkan_library.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-08-03
///
/// The contets were based on the Vulkan Cookbook 2017 by Pawel Lapinski
/// ISBN: 9781786468154
///
/// \brief

#ifndef CIRCE_VULKAN_LIBRARY
#define CIRCE_VULKAN_LIBRARY

#include <array>
#include <cstring>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#if defined _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#define VulkanLibraryType HMODULE
#elif __linux
#define VulkanLibraryType void *
#elif __APPLE__
#define VulkanLibraryType void *
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace circe::vk {

/// Holds a handle to the Vulkan API. Provides a set of helper functions
/// to perform all Vulkan operations.
class VulkanLibraryInterface {
public:
  /// Stores information about queues requested to a logical device and the list
  /// of priorities assifned to each queue
  struct QueueFamilyInfo {
    std::optional<uint32_t> family_index; //!< queue family index
    std::vector<float> priorities; //!< list of queue priorities, [0.0,1.0]
  };
  /// Stores swap chain support information:
  /// - Basic surface capabilities (min/max number of images in swap chain,
  /// min/max width and height of images)
  /// - Surface formats (pixel format, color space)
  /// - Available presentation modes
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  };
  ///
  struct PhysicalDevice {
    VkPhysicalDevice handle;
    VkPhysicalDeviceFeatures &features;
    VkPhysicalDeviceProperties &properties;
    std::vector<VkQueueFamilyProperties> queue_families;
  };
  ///
  struct WindowParameters {
#ifdef VK_USE_PLATFORM_WIN32_KHR
    HINSTANCE HInstance;
    HWND HWnd;
#elif defined VK_USE_PLATFORM_XLIB_KHR
    Display *Dpy;
    Window Window;
#elif defined VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t *Connection;
    xcb_window_t Window;
#endif
  };
  /// Used in the presentation of an image in a given swapchain, for each
  /// swapchain only one image can be presented at a time.
  struct PresentInfo {
    VkSwapchainKHR Swapchain;
    uint32_t ImageIndex;
  };
  /// Stores a semaphore on which hardware should wait and on what pipeline
  /// stages the wait should occur.
  struct WaitSemaphoreInfo {
    VkSemaphore Semaphore;
    VkPipelineStageFlags WaitingStage;
  };
  /// Defines parameters to use for buffer memory barrier
  struct BufferTransition {
    VkBuffer Buffer;
    VkAccessFlags CurrentAccess; //!< how the buffer has been used so far
    VkAccessFlags NewAccess;     //!< how the buffer will be used from now on
    uint32_t CurrentQueueFamily; //!< queue family owning the buffer
    uint32_t NewQueueFamily; //!< queue family that will receive the ownership
  };
  /// Defines parameters to use for image memory barrier
  struct ImageTransition {
    VkImage Image;
    VkAccessFlags CurrentAccess; //!< how the image has been used so far
    VkAccessFlags NewAccess;     //!< how the image will be used from now on
    VkImageLayout CurrentLayout; //!< how the image has been organized in memory
    VkImageLayout NewLayout;     //!< how the image will be organized in memory
    uint32_t CurrentQueueFamily; //!< queue family owning the image
    uint32_t NewQueueFamily; //!< queue family that will receive the ownership
    VkImageAspectFlags
        Aspect; //!< image's usage contex (color, depth or stencil)
  };
  /// Defines parameters for shader stages definitions
  struct ShaderStageParameters {
    VkShaderStageFlagBits ShaderStage; //!< pipeline stage
    VkShaderModule ShaderModule;       //!< module with the shader code
    char const *EntryPointName; //!< function name associated with the stage
    VkSpecializationInfo const
        *SpecializationInfo; //!< allows modification of constant variables
                             //!< defined in shader
  };
  /// Viewport descriptions
  struct ViewportInfo {
    std::vector<VkViewport> Viewports; //!< parameters for a set of viewports_
    std::vector<VkRect2D> Scissors;    //!< parameters for scissor tests
                                       //!< corresponding to each viewport
  };
  // Specifies the buffer's parameters for buffer binding
  struct VertexBufferParameters {
    VkBuffer Buffer;
    VkDeviceSize MemoryOffset;
  };

  // VULKAN API FUNCTION LOADING
  // ---------------------------
  // Vulkan API provides a helper function that can be used to load its function
  // pointers across different platforms. Vulkan functions can be divided into 3
  // levels: global, instance and device.
  // Device-level: drawing, shader module creation, data copying.
  // Instance-level: create logical devices.
  // Global-level: load device and instance-level functions.

  /// Retrieves the pointer to the helper Vulkan API function that is used to
  /// load all other functions across platforms.
  /// \param vulkan_library **[out]** function loader's pointer.
  /// \return true if success
  static bool loadLoaderFunctionFromVulkan(VulkanLibraryType &vulkan_library);
  /// Since we load the Vulkan Library dinamically, we must explicitly release
  /// it.
  /// \param vulkan_library **[in/out]** function loader's pointer.
  static void releaseVulkanLoaderLibrary(VulkanLibraryType &vulkan_library);
  /// Loads all global level functions listed in the vulkan_api files.
  /// Global-level functions can be used to perform operations such as drawing,
  /// shader modules creation, data copying.
  /// \return bool true if success
  static bool loadGlobalLevelFunctions();
  /// Loads all instance level functions listed in the vulkan_api files.
  /// Instance-level functions can be used to create logical devices.
  /// \param instance **[in]**
  /// \param extensions **[in]**
  /// \return bool true if success
  static bool
  loadInstanceLevelFunctions(VkInstance instance,
                             const std::vector<const char *> &extensions);
  /// Loads all device level functions listed in the vulkan_api files.
  /// Device-level functions can be used for rendering, calculating collisions
  /// of objects, processing video frames, etc.
  /// \param logical_device **[in]** logical device handle
  /// \param enabled_extensions **[in]** enabled extensions for the logical
  /// device
  /// \return bool true if success
  static bool
  loadDeviceLevelFunctions(VkDevice logical_device,
                           std::vector<char const *> const &enabled_extensions);

  // VULKAN API EXTENSIONS
  // ---------------------
  // Since Vulkan is an platform agnostic api, it means we need extensions
  // to communicate with system specifics features. For example, an extension
  // to interface the application with the window system.
  // There are 2 levels of extensions: intance-level and device-level.
  // Instance Level Extensions:   enabled on instance creation
  // Device Level Extensions: enabled on logical device creation
  // In order to create a Vulkan Instance(or logical device) with the desired
  // extensions, we first need to check if the extensions are supported on the
  // current hardware platform. This is done by checking if the extensions we
  // want are listed by the respective supported extensions.

  /// Gets the list of the properties of supported instance extensions on the
  /// current hardware platform.
  /// \param extensions **[out]** list of extensions
  /// \return bool true if success
  static bool checkAvaliableInstanceExtensions(
      std::vector<VkExtensionProperties> &extensions);
  /// Gets the list of the properties of supported extensions for the device.
  /// \param physical_device **[in]** physical device handle
  /// \param extensions **[out]** list of extensions
  /// \return bool true if success
  static bool checkAvailableDeviceExtensions(
      VkPhysicalDevice physical_device,
      std::vector<VkExtensionProperties> &extensions);
  /// Checks if **extension_name** is listed by **extensions**
  /// \param extensions **[in]** list of supported extensions
  /// \param extension_name **[in]** extension name
  /// \return bool true if extension is a supported extension
  static bool
  isExtensionSupported(const std::vector<VkExtensionProperties> &extensions,
                       const char *extension_name);

  // VULKAN INSTANCE
  // ---------------
  // The Vulkan Instance holds all kind of information about the application,
  // such as application name, version, etc. The instance is the interface
  // between the application and the Vulkan Library, that can perform
  // operations like the enumeration of available physical devices and creation
  // of logical devices.

  /// \brief Create a Vulkan Instance object
  /// A Vulkan Instance represents the application state and connects the
  /// application with the Vulkan Library.
  /// \param extensions **[in]** list of desired instance extensions
  /// \param application_name **[in]** application's name
  /// \param instance **[out]** vulkan instance handle
  /// \return bool if success
  static bool createInstance(const std::vector<const char *> &extensions,
                             std::string application_name,
                             VkInstance &instance);
  /// Clean up the resources used by the Vulkan Instance and destroys its
  /// handle. Must be called after the destruction of all its dependencies
  /// (logical devices).
  /// \param instance **[in/out]** Vulkan Instance handle
  static void destroyVulkanInstance(VkInstance &instance);

  // VULKAN PHYSICAL DEVICE
  // ----------------------
  // Physical devices are the hardware we intend to use with Vulkan. Thus we
  // need to look for the devices that supports the features we need. We can
  // select any number of graphics cards and use them simultaneously.
  // The Vulkan Library allows us to get devices capabilities and properties so
  // we can select the one that best suits for our application.

  /// Enumarates the available physical devices in the system that support
  /// the requirements of the application.
  /// \param instance **[in]** instance object (describing our needs)
  /// \param devices **[out]** physical device handles
  /// \return bool true if success
  static bool
  enumerateAvailablePhysicalDevices(VkInstance instance,
                                    std::vector<VkPhysicalDevice> &devices);
  /// Retrieves the physical device capabilities and features. Device properties
  /// describe general information surch as name, version of a driver, type of
  /// the device (integrated or discrete), memory, etc. Device features include
  /// items such as geometry and tesselation shaders, depth clamp, etc.
  /// \param physical_device **[in]** physical device handle
  /// \param device_features **[out]** device features
  /// \param device_properties **[out]** device properties
  static void getFeaturesAndPropertiesOfPhysicalDevice(
      VkPhysicalDevice physical_device,
      VkPhysicalDeviceFeatures &device_features,
      VkPhysicalDeviceProperties &device_properties);
  /// Acquires physical device memory properties, such as number of heaps, their
  /// sizes, types and etc.
  /// \param physical_device **[in]** physical device handler
  /// \param properties **[out]** physical device memory properties
  static void getPhysicalDeviceMemoryProperties(
      VkPhysicalDevice physical_device,
      VkPhysicalDeviceMemoryProperties &properties);

  // VULKAN QUEUE FAMILIES
  // ---------------------
  // Every Vulkan operation requires commands that are submitted to a queue.
  // Different queues can be processed independently and may support different
  // types of operations.
  // Queues with the same capabilities are grouped into families. A device may
  // expose any number of queue families. For example, there could be a queue
  // family that only allows processing of compute commands or one that only
  // allows memory transfer related commands.

  /// Retrieve available queue families exposed by a physical device
  /// \param physical_device **[in]** physical device handle
  /// \param queue_families **[in]** list of exposed queue families
  /// \return bool true if success
  static bool checkAvailableQueueFamiliesAndTheirProperties(
      VkPhysicalDevice physical_device,
      std::vector<VkQueueFamilyProperties> &queue_families);
  /// Given a physical device, finds the queue family index that supports the
  /// desired set of capabilities.
  /// \param physical_device **[in]** physical device handle
  /// \param desired_capabilities **[in]** desired set of capabalities
  /// \param queue_family_index **[in]** capable queue family index
  /// \return bool true if success
  static bool selectIndexOfQueueFamilyWithDesiredCapabilities(
      VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities,
      uint32_t &queue_family_index);
  /// Finds a queue family of a physical device that can accept commands for a
  /// given surface
  /// \param physical_device **[in]** physical device handle
  /// \param presentation_surface **[in]** surface handle
  /// \param queue_family_index **[out]** queue family index
  /// \return bool true if success
  static bool selectQueueFamilyThatSupportsPresentationToGivenSurface(
      VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface,
      uint32_t &queue_family_index);

  // VULKAN LOGICAL DEVICE
  // ---------------------
  // The logical device makes the interface of the application and the physical
  // device. Represents the hardware, along with the extensions and features
  // enabled for it and all the queues requested from it. The logical device
  // allows us to record commands, submit them to queues and acquire the
  // results.
  // Device queues need to be requested on device creation, we cannot create or
  // destroy queues explicitly. They are created/destroyed on logical device
  // creation/destruction.

  /// \brief Creates a Logical Device handle
  /// \param physical_device **[in]** physical device handle
  /// \param queue_infos **[in]** queues description
  /// \param desired_extensions **[in]** desired extensions
  /// \param desired_features **[in]** desired features
  /// \param logical_device **[out]** logical device handle
  /// \return bool true if success
  static bool
  createLogicalDevice(VkPhysicalDevice physical_device,
                      std::vector<QueueFamilyInfo> queue_infos,
                      std::vector<char const *> const &desired_extensions,
                      VkPhysicalDeviceFeatures *desired_features,
                      VkDevice &logical_device);
  /// Clean up the resources used by the logical device and destroys its handle.
  /// Must be destroyed before destroy the Vulkan Instance.
  /// \param logical_device **[in/out]** logical device handle
  static void destroyLogicalDevice(VkDevice &logical_device);
  /// \param logical_device **[in]** logical device handle
  /// \param queue_family_index **[in]** queue family index
  /// \param queue_index **[in]** queue index
  /// \param queue **[out]** queue
  static void getDeviceQueue(VkDevice logical_device,
                             uint32_t queue_family_index, uint32_t queue_index,
                             VkQueue &queue);

  // VULKAN RESOURCES AND MEMORY
  // ---------------------------
  // Vulkan uses memory objects to provide resources such as buffers and images.
  // Buffers represent linear arrays and images can represent up to
  // three-dimensional data arranged in ways specific to hardware. These
  // resources serve to various purposes (such as shader data, render targets,
  // etc). These usages must be specified on resource's creation.
  // The driver must be informed about the usage not only during the
  // buffer/image creation, but also before the actual usage. It is because we
  // might want to change the usage of the buffer/image during execution. This
  // is done by memory barriers, which are set as part of the pipeline barriers
  // during command buffer recording.
  // Memory barriers are very important to assure that commands read content
  // from buffers/images properly, i.e. that the commands that write into the
  // memory finish their job before the read.
  // Vulkan also provides image layouts. Depending on the usage of the image,
  // it may be organized in memory differently to optimize access to it. We
  // can also change the image layout during an image memory barrier.

  /// Iterates over available physical device's memory types and check against
  /// the desired memory properties (number of heaps, their sizes and types),
  /// then allocate and binds it to the given buffer.
  /// \param physical_device **[in]** physical device handle which the logical
  /// device was created
  /// \param logical_device **[in]** logical device handle created from the
  /// physical device
  /// \param buffer **[in]** buffer handle
  /// \param memory_properties **[in]** desired memory properties
  /// \param memory_object **[out]** allocated memory object
  /// \return bool true if success
  static bool allocateAndBindMemoryObjectToBuffer(
      VkPhysicalDevice physical_device, VkDevice logical_device,
      VkBuffer buffer, VkMemoryPropertyFlagBits memory_properties,
      VkDeviceMemory &memory_object);
  /// Setups buffer memory barriers
  /// \param command_buffer **[in]** command buffer handle (in recording stage)
  /// \param generating_stages **[in]** pipeline stages that have been using the
  /// buffer so far
  /// \param consuming_stages **[in]** pipeline stages in which the buffer will
  /// be used after the barrier
  /// \param buffer_transitions **[in]** parameters for each buffer that a
  /// barrier will be set up for
  void setBufferMemoryBarrier(VkCommandBuffer command_buffer,
                              VkPipelineStageFlags generating_stages,
                              VkPipelineStageFlags consuming_stages,
                              std::vector<BufferTransition> buffer_transitions);
  /// Iterates over available physical device's memory types and check against
  /// the desired memory properties (number of heaps, their sizes and types),
  /// then allocate and binds it to the given image.
  /// \param physical_device **[in]** physical device handle which the logical
  /// device was created
  /// \param logical_device **[in]** logical device handle created from the
  /// physical device
  /// \param image **[in]** image handle
  /// \param memory_properties **[in]** desired memory properties
  /// \param memory_object **[out]** allocated memory object
  /// \return bool true if success
  static bool
  allocateAndBindMemoryObjectToImage(VkPhysicalDevice physical_device,
                                     VkDevice logical_device, VkImage image,
                                     VkMemoryPropertyFlagBits memory_properties,
                                     VkDeviceMemory &memory_object);
  /// Setups image memory barriers
  /// \param command_buffer **[in]** command buffer handle (in recording stage)
  /// \param generating_stages **[in]** pipeline stages that have been using the
  /// image so far
  /// \param consuming_stages **[in]** pipeline stages in which the image will
  /// be used after the barrier
  /// \param image_transitions **[in]** parameters for each image that a
  /// barrier will be set up for
  static void
  SetImageMemoryBarrier(VkCommandBuffer command_buffer,
                        VkPipelineStageFlags generating_stages,
                        VkPipelineStageFlags consuming_stages,
                        std::vector<ImageTransition> image_transitions);
  /// \param logical_device **[in]** logical device handle
  /// \param buffer **[in/out]** buffer handle
  static void destroyBuffer(VkDevice logical_device, VkBuffer &buffer);
  /// \param logical_device **[in]** logical device handle
  /// \param image **[in/out]** image handle
  static void destroyImage(VkDevice logical_device, VkImage &image);
  /// \param logical_device **[in]** logical device handle
  /// \param memory_object **[in/out]** memory object
  static void freeMemoryObject(VkDevice logical_device,
                               VkDeviceMemory &memory_object);

  // VULKAN BUFFER VIEW
  // -----------------
  // Buffer views allow us to define how buffer's memory is accessed and
  // interpreted. For example, we can choose to look at the buffer as a uniform
  // texel buffer or as a storage texel buffer.

  /// Creates a buffer view of a portion of the given buffer
  /// \param logical_device **[in]** logical device handle
  /// \param buffer **[in]** buffer handle
  /// \param format **[in]** how buffer contents should be interpreted
  /// \param memory_offset **[in]** view's starting point
  /// \param memory_range **[in]** size of the view
  /// \param buffer_view **[out]** buffer view object
  /// \return bool true if success
  static bool createBufferView(VkDevice logical_device, VkBuffer buffer,
                               VkFormat format, VkDeviceSize memory_offset,
                               VkDeviceSize memory_range,
                               VkBufferView &buffer_view);
  /// \param logical_device **[in]** logical device handle
  /// \param buffer_view **[in/out]** buffer view handle
  static void destroyBufferView(VkDevice logical_device,
                                VkBufferView &buffer_view);

  // VULKAN IMAGE VIEW
  // -----------------
  // Image views define a selected part of an image's memory and specify
  // additional information needed to properly read an image's data.

  /// Creates an image view object for the entire given image
  /// \param logical_device **[in]** logical device handle
  /// \param image **[in]** image handle
  /// \param view_type **[in]**
  /// \param format **[in]** data format
  /// \param aspect **[in]** context: color, depth or stencil
  /// \param image_view **[out]** image view object
  /// \return bool true if success
  static bool createImageView(VkDevice logical_device, VkImage image,
                              VkImageViewType view_type, VkFormat format,
                              VkImageAspectFlags aspect,
                              VkImageView &image_view);
  /// \param logical_device **[in]** logical device handle
  /// \param image_view **[in/out]** image view handle
  static void destroyImageView(VkDevice logical_device,
                               VkImageView &image_view);

  // VULKAN SURFACE
  // --------------
  // Vulkan does not provide a way to display images in the application's
  // window by default. We need extensions to do so. These extensions are
  // commonly referred as Windowing System Integration (WSI) and each
  // operating system has its own set of extensions. The presentation surface
  // is the Vulkan representation of an application's window. Instance-level
  // extensions are responsable for managing, creating, and destroying a
  // presentation surface.

  /// \brief Create a Presentation Surface handle
  /// \param instance **[in]** vulkan instance handle
  /// \param window_parameters **[in]** window parameters
  /// \param presentation_surface **[out]** presentation surface handle
  /// \return bool true if success
  static bool createPresentationSurface(VkInstance instance,
                                        WindowParameters window_parameters,
                                        VkSurfaceKHR &presentation_surface);
  /// \param instance **[in]** instance handle
  /// \param presentation_surface **[in/out]** surface handle
  static void destroyPresentationSurface(VkInstance instance,
                                         VkSurfaceKHR &presentation_surface);
  /// \brief Retrieve the capabilities of the surface
  /// \param physical_device **[in]** physical device handle
  /// \param presentation_surface **[in]** surface handle
  /// \param surface_capabilities **[out]** surface capabilities
  /// \return bool true if success
  static bool getCapabilitiesOfPresentationSurface(
      VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface,
      VkSurfaceCapabilitiesKHR &surface_capabilities);

  // VULKAN SWAP CHAIN
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

  /// Checks if the desired presentation mode is supported by the device, if
  /// so, it is returned in **present_mode**. If not, VK_PRESENT_MODE_FIFO_KHR
  /// is chosen. \param physical_device **[in]** physical device handle \param
  /// presentation_surface **[in]** surface handle \param desired_present_mode
  /// **[in]** described presentation mode \param present_mode **[out]**
  /// available presentation mode \return bool true if success
  static bool selectDesiredPresentationMode(
      VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface,
      VkPresentModeKHR desired_present_mode, VkPresentModeKHR &present_mode);
  /// \brief Gets swap chain support information for a surface in a given
  /// physical device.
  /// \param physical_device **[in]** physical device handle
  /// \param presentation_surface **[in]** surface handle
  /// \param details **[out]** support information
  /// \return bool true if success
  static bool querySwapChainSupport(VkPhysicalDevice physical_device,
                                    VkSurfaceKHR surface,
                                    SwapChainSupportDetails &details);
  /// Computes the minimal value + 1, of images required for the presentation
  /// engine to work properly.
  /// \param surface_capabilities **[in]** surface capabilities
  /// \param number_of_images **[out]** available number of images
  /// \return bool true if success
  static bool selectNumberOfSwapchainImages(
      const VkSurfaceCapabilitiesKHR &surface_capabilities,
      uint32_t &number_of_images);
  /// Clamps the **size_of_images** to the maximum supported by the surface
  /// capabilities.
  /// \param surface_capabilities **[in]** surface capabilities
  /// \param size_of_images **[in/out]** desired/available size of images
  /// \return bool true if success
  static bool chooseSizeOfSwapchainImages(
      VkSurfaceCapabilitiesKHR const &surface_capabilities,
      VkExtent2D &size_of_images);
  /// \brief Clamps the usage flag to the surface capabilities
  /// Images can also be used for purposes other than as color attachments.
  /// For example, we can sample from them and use them in copy operations.
  /// \param surface_capabilities **[in]** surface capabilities
  /// \param desired_usages **[in]** desired usages
  /// \param image_usage **[in/out]** available usages
  /// \return bool true if success
  static bool selectDesiredUsageScenariosOfSwapchainImages(
      VkSurfaceCapabilitiesKHR const &surface_capabilities,
      VkImageUsageFlags desired_usages, VkImageUsageFlags &image_usage);
  /// \brief Clamps the desired image orientation to the supported image
  /// orientation.
  /// On some devices, images can be displayed in different orientations.
  /// \param surface_capabilities **[in]** surface capabilities
  /// \param desired_transform **[in]** desired image orientation
  /// \param surface_transform **[in]** available image orientation
  /// \return bool true if success
  static bool selectTransformationOfSwapchainImages(
      VkSurfaceCapabilitiesKHR const &surface_capabilities,
      VkSurfaceTransformFlagBitsKHR desired_transform,
      VkSurfaceTransformFlagBitsKHR &surface_transform);
  /// Clamps the desired image format to the supported format by the
  /// device. The format defines the number of color components, the number of
  /// bits for each component and data type. Also, we must specify the color
  /// space to be used for encoding color.
  /// \param physical_device **[in]** physical device handle
  /// \param presentation_surface **[in]** surface handle
  /// \param desired_surface_format **[in]** desired image format
  /// \param image_format **[out]** available image format
  /// \param image_color_space **[out]** available color space
  /// \return bool true if success
  static bool selectFormatOfSwapchainImages(
      VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface,
      VkSurfaceFormatKHR desired_surface_format, VkFormat &image_format,
      VkColorSpaceKHR &image_color_space);
  /// \brief Create a SwapChain object
  /// \param logical_device **[in]** logical device handle
  /// \param presentation_surface **[in]** surface handle
  /// \param image_count **[in]** swapchain image count
  /// \param surface_format **[in]** surface format
  /// \param image_size **[in]** image size
  /// \param image_usage **[in]** image usages
  /// \param surface_transform **[in]** surface transform
  /// \param present_mode **[in]** presentation mode
  /// \param old_swapchain **[in | optional]** old swap chain handle
  /// \param swapchain **[out]** new swap chain handle
  /// \return bool true if success
  static bool
  createSwapchain(VkDevice logical_device, VkSurfaceKHR presentation_surface,
                  uint32_t image_count, VkSurfaceFormatKHR surface_format,
                  VkExtent2D image_size, VkImageUsageFlags image_usage,
                  VkSurfaceTransformFlagBitsKHR surface_transform,
                  VkPresentModeKHR present_mode, VkSwapchainKHR &old_swapchain,
                  VkSwapchainKHR &swapchain);
  /// Needs to be destroyed before the presentation surface
  /// \param logical_device **[in]** logical device handle
  /// \param swapchain **[in/out]** swapchain handle
  static void destroySwapchain(VkDevice logical_device,
                               VkSwapchainKHR &swapchain);

  // SWAPCHAIN IMAGE OPERATIONS
  // --------------------------
  // In order to use the images contained in the swapchain, we need to acquire
  // them first. When acquiring images, we can use semaphores and fences.
  // Semaphores can be used in internal queue synchronization. Fences are used
  // to synchronize the queues and the application.
  // After we use the image, we need to give it back to the presentation
  // engine so it can be displayed on screen. The type of access on the images
  // is described by the image view object, which defines the portion of the
  // image to be accessed and how it will be accessed (for example, if it
  // should be treated as a 2D depth texture with mipmap levels).

  /// \brief Get the handle list of swapchain images
  /// \param logical_device **[in]** logical device handle
  /// \param swapchain **[in]** swapchain handle
  /// \param swapchain_images **[out]** list of swapchain image handles
  /// \return bool true if success
  static bool
  getHandlesOfSwapchainImages(VkDevice logical_device, VkSwapchainKHR swapchain,
                              std::vector<VkImage> &swapchain_images);
  /// Acquires an image index (in the array returned by the
  /// **getHandlesOfSwapchainImages** method). The fence is used to make sure
  /// the application does not modify the image while there are still
  /// previously submitted operations happening on the image. The semaphore is
  /// used to tell the driver to not start processing new commands with the
  /// given image.
  /// \param logical_device **[in]** logical device handle
  /// \param swapchain **[in]** swapchain handle
  /// \param semaphore **[in]** semaphore handle
  /// \param fence **[in]** fence handle
  /// \param image_index **[out]** acquired image index \return bool true if
  /// success
  static bool acquireSwapchainImage(VkDevice logical_device,
                                    VkSwapchainKHR swapchain,
                                    VkSemaphore semaphore, VkFence fence,
                                    uint32_t &image_index);
  /// \brief Sends to the hardware the images to be presented. Semaphores are
  /// used to assure the correct display time for each image. Multiple images
  /// can be presented at the same time, but only one per swapchain.
  /// \param queue **[in]** the handle of a queue that supports presentation
  /// \param rendering_semaphores **[in]** associated semaphore for each image
  /// list
  /// \param images_to_present **[in]** list of images
  static bool presentImage(VkQueue queue,
                           std::vector<VkSemaphore> rendering_semaphores,
                           std::vector<PresentInfo> images_to_present);

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

  /// \brief Creates a semaphore for a given logical device
  /// \param logical_device **[in]** logical device handle
  /// \param semaphore **[out]** semaphore handle
  /// \return bool true if success
  static bool createSemaphore(VkDevice logical_device, VkSemaphore &semaphore);
  /// \param logical_device **[in]** logical device handle
  /// \param semaphore **[in/out]** semaphore handle
  static void destroySemaphore(VkDevice logical_device, VkSemaphore &semaphore);
  /// \brief Creates a fence for a given logical device.
  /// \param logical_device **[in]** logical device handle
  /// \param signaled **[in]** fence's initial state
  /// \param fence **[out]** fence handle
  /// \return bool true if success
  static bool createFence(VkDevice logical_device, bool signaled,
                          VkFence &fence);
  /// \param logical_device **[in]** logical device handle
  /// \param fence **[in/out]** fence handle
  static void destroyFence(VkDevice logical_device, VkFence &fence);
  /// \brief Blocks application until the fence(s) get changed or time out.
  /// \param logical_device **[in]** logical device handle
  /// \param fences **[in]** list of fences
  /// \param wait_for_all **[in]** **false** if any fence that gets changed is
  /// enough, or **true** if all fences must be changed.
  /// \param timeout **[in]** time out
  /// \return bool true if success
  static bool waitForFences(VkDevice logical_device,
                            std::vector<VkFence> const &fences,
                            VkBool32 wait_for_all, uint64_t timeout);
  /// \brief Deactivate the given fences. Its the application's responsability
  /// to put the fences back to their initial state after they get activated.
  /// \param logical_device **[in]** logical device handle
  /// \param fences **[in/out]** list of fences
  /// \return bool true if success
  static bool resetFences(VkDevice logical_device,
                          std::vector<VkFence> const &fences);

  // VULKAN COMMAND BUFFERS
  // ----------------------
  // Command buffers record operations and are submitted to the hardware. They
  // can be recorded in multiple threads and also can be saved and reused.
  // Synchronization is very important on this part, because the operations
  // submitted need to be processed properly.
  // Before allocating command buffers, we need to allocate command pools,
  // from which the command buffers acquire memory. Command pools are also
  // responsible for informing the driver on how to deal with the command
  // buffers memory allocated from them (for example, whether the command
  // buffer will have a short life or if they need to be reset or freed).
  // Command pools also control the queues that receive the command buffers.
  // Command buffers are separate in two groups:
  // 1. Primary - can be directly submitted to queues and call secondary
  // command
  //    buffers.
  // 2. Secondary - can only be executed from primary command buffers.
  // When recording commands to command buffers, we need to set the state for
  // the operations as well (for example, vertex attributes use other buffers
  // to work). When calling a secondary command buffer, the state of the
  // caller primary command buffer is not preserved (unless it is a render
  // pass).

  /// \brief Create a Command Pool object
  /// Command pools cannot be used concurrently, we must create a separate
  /// command pool for each thread.
  /// \param logical_device **[in]** logical device handle
  /// \param parameters **[in]** flags that define how the command pool will
  /// handle command buffer creation, memory, life span, etc.
  /// \param queue_family **[in]** queue family to which command buffers will
  /// be submitted. \param command_pool **[out]** command pool handle \return
  /// bool
  static bool createCommandPool(VkDevice logical_device,
                                VkCommandPoolCreateFlags parameters,
                                uint32_t queue_family,
                                VkCommandPool &command_pool);
  /// \param logical_device **[in]** logical device handle
  /// \param command_pool **[in/out]** command pool handle
  static void destroyCommandPool(VkDevice logical_device,
                                 VkCommandPool &command_pool);
  /// Creates a list of command buffers from a given command pool.
  /// \param logical_device **[in]** logical device handle
  /// \param command_pool **[in]** command pool handle
  /// \param level **[in]** command buffers level (Primary or Secondary)
  /// \param count **[in]** number of command buffers
  /// \param command_buffers **[out]** list of command buffer handles
  /// \return bool true if success
  static bool
  allocateCommandBuffers(VkDevice logical_device, VkCommandPool command_pool,
                         VkCommandBufferLevel level, uint32_t count,
                         std::vector<VkCommandBuffer> &command_buffers);
  /// \brief Puts the command buffer in record state and allow vkCmd*
  /// functions to be recorderd. \param command_buffer **[in]** command buffer
  /// handle \param usage **[in]** usage description \param
  /// secondary_command_buffer_info **[in]** \return bool true if success
  static bool beginCommandBufferRecordingOperation(
      VkCommandBuffer command_buffer, VkCommandBufferUsageFlags usage,
      VkCommandBufferInheritanceInfo *secondary_command_buffer_info);
  /// \brief Stops recording commands by taking the command buffer out of the
  /// recording state.
  /// \param command_buffer **[in]** command buffer handle
  /// \return bool true if success
  static bool
  endCommandBufferRecordingOperation(VkCommandBuffer command_buffer);
  /// \brief Explicitly reset the command buffer to allow a new recording.
  /// \param command_buffer **[in]** command buffer handle
  /// \param release_resources **[in]** set to give back the memory to the
  /// memory pool
  /// \return bool true if success
  static bool resetCommandBuffer(VkCommandBuffer command_buffer,
                                 bool release_resources);
  /// \brief Resets all command buffers of a given command pool at once.
  /// \param logical_device **[in]** logical device
  /// \param command_pool **[in]** command pool handle
  /// \param release_resources **[in]** give memory from the command buffers
  /// back to the pool
  /// \return bool true if success
  static bool resetCommandPool(VkDevice logical_device,
                               VkCommandPool command_pool,
                               bool release_resources);
  /// \param logical_device **[in]** logical device handle
  /// \param command_pool **[in]** command pool handle
  /// \param command_buffers **[in]** list of command buffer handles
  static void freeCommandBuffers(VkDevice logical_device,
                                 VkCommandPool command_pool,
                                 std::vector<VkCommandBuffer> &command_buffers);

  // COMMAND BUFFER SUBMISSION
  // -------------------------
  // When submitting a command buffer, we can choose semaphores on which the
  // device should wait before processing the command buffer and also in which
  // pipeline stages the wait should occur.

  /// \brief Submittes command buffers to a queue following waiting rules.
  /// \param queue **[in]** queue handle
  /// \param wait_semaphore_infos **[in]** waiting information
  /// \param command_buffers **[in]** list of command buffer handles
  /// \param signal_semaphores **[in]** list of semaphores on which the queue
  /// should wait to execute the command buffers
  /// \param fence **[in]** fance handle to be signaled after all command
  /// buffers get executed
  /// \return bool true if success
  static bool submitCommandBuffersToQueue(
      VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos,
      std::vector<VkCommandBuffer> command_buffers,
      std::vector<VkSemaphore> signal_semaphores, VkFence fence);
  /// \param queue **[in]** queue handle
  /// \return bool true if success
  static bool waitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue);
  /// \param logical_device **[in]** logical device handle
  /// \return bool true if success
  static bool waitForAllSubmittedCommandsToBeFinished(VkDevice logical_device);

  // VULKAN PIPELINES
  // ----------------
  // The operations recorded in command buffers are processed by the hardware in
  // a pipeline. Pipeline objects control the way in which computations are
  // performed. Different from OpenGL though, the whole pipeline state is stored
  // in a single object (In OpenGL we have a state machine where we can activate
  // pieces separately and switch between resources during execution). In
  // Vulkan, each configuration will require its own pipeline object (for
  // example, if we want to switch between shaders, we need to prepare the whole
  // pipeline for the new set of shaders).
  // The computations executed inside the pipeline are performed by shaders.
  // Shaders are represented by Shader Modules and must be provided to Vulkan as
  // SPIR-V assembly code. A single module may contain code for multiple shader
  // stages.
  // The interface between shader stages and shader resources is specified
  // through pipeline layouts (for example, the same address needs to be used in
  // shaders).
  // There are two types of pipelines:
  // - Graphics pipelines
  //    Are used for drawing when binded to the command buffer before recording
  //    a drawing command. Can be bounded only inside render passes.
  // - Compute pipelines
  //    Consisted of a single compute shader stage, compute pipelines are used
  //    to perform mathematical operations.

  /// \param logical_device **[in]** logical device handle
  /// \param source_code **[in]** binary SPIR-V assembly of the shader(s)
  /// \param shader_module **[out]** shader module object
  /// \return bool true if success
  static bool createShaderModule(VkDevice logical_device,
                                 std::vector<unsigned char> const &source_code,
                                 VkShaderModule &shader_module);
  /// Specifies the set of shader stages that will be active in a pipeline.
  /// Graphics pipelines for example, may contain geometry, tesselation and
  /// other stages.
  /// \param shader_stage_params **[in]** description of stages active in the
  /// pipeline
  /// \param shader_stage_create_infos **[out]** pipeline stage creation infos
  static void specifyPipelineShaderStages(
      std::vector<ShaderStageParameters> const &shader_stage_params,
      std::vector<VkPipelineShaderStageCreateInfo> &shader_stage_create_infos);
  /// \brief Specifies pipeline vertex input state
  /// The vertex input stages prepare vertex data by associating vertices to its
  /// attributes and describing how they are stored in memory.
  /// \param binding_descriptions **[in]** descriptions for each vertex binding
  /// \param attribute_descriptions **[in]** vertex attributes provided to the
  /// vertex shader
  /// \param vertex_input_state_create_info **[out]** vertex input state
  /// creation info object
  static void specifyPipelineVertexInputState(
      std::vector<VkVertexInputBindingDescription> const &binding_descriptions,
      std::vector<VkVertexInputAttributeDescription> const
          &attribute_descriptions,
      VkPipelineVertexInputStateCreateInfo &vertex_input_state_create_info);
  /// \brief Specifies pipeline input assembly state
  /// Input assembly stages are resposible for assembling vertices into
  /// primitives.
  /// \param topology **[in]** type of primitives to be formed from vertices
  /// \param primitive_restart_enable **[in]** whether a special index value
  /// should restart a primitive (can't be used for list primitives)
  /// \param input_assembly_state_create_info **[out]** assembly state creation
  /// info object
  static void specifyPipelineInputAssemblyState(
      VkPrimitiveTopology topology, bool primitive_restart_enable,
      VkPipelineInputAssemblyStateCreateInfo &input_assembly_state_create_info);
  /// \brief Specifies pipeline tesselation state
  /// The tesselation stages generate patchs of geometry to produce more refined
  /// meshes.
  /// Note: to use tesselation shaders the tesselationShader feature must be
  /// enabled during logical device creation. Code for both tesselation control
  /// and evaluation shaders.
  /// \param patch_control_points_count **[in]** number of vertices which form a
  /// patch
  /// \param tessellation_state_create_info **[out]** tesselation state creation
  /// info
  static void specifyPipelineTessellationState(
      uint32_t patch_control_points_count,
      VkPipelineTessellationStateCreateInfo &tessellation_state_create_info);
  /// \brief Specifies pipeline viewport and scissor test state
  /// Viewports and scissor tests define the area of the image to which we want
  /// to draw. These stages transform vertices into clip space and assemble
  /// polygons for rasterization and fragment creation. Here we define the
  /// rendering area and depth values. The scissor test can define rendering
  /// areas smaller than the viewport. Viewport and scissor test can be defined
  /// dynamic, so there is no need to recreate the pipeline when they change
  /// (during command buffer recording). However, if their number is changed the
  /// pipeline must be recreated.
  /// \param viewport_infos **[in]** viewports_ description
  /// \param viewport_state_create_info **[out]** viewport state creation info
  /// object
  static void specifyPipelineViewportAndScissorTestState(
      ViewportInfo const &viewport_infos,
      VkPipelineViewportStateCreateInfo &viewport_state_create_info);
  /// \brief Specifies the pipeline rasterization state
  /// The rasterizations stages produce fragments from the assembled polygons.
  /// Here we can specify the polygon's front side, control culling, draw mode
  /// (polygon fill, only edges, etc), define how depth value for fragments are
  /// generated, width of lines and more.
  /// \param depth_clamp_enable **[in]** whether fragments with out of range
  /// depth values should be clamped (true) or clipped(false)
  /// \param rasterizer_discard_enable **[in]** whether fragments should be
  /// normally generated (false) or disable the rasterization (true)
  /// \param polygon_mode **[in]** draw mode (fully filled or if lines or
  /// points)
  /// \param culling_mode **[in]** polygon visible sides
  /// \param front_face **[in]** polygon's vertex order: cc or ccw
  /// \param depth_bias_enable **[in]** whether the depth values are incremented
  /// by an offset (true)
  /// \param depth_bias_constant_factor **[in]** depth bias offset
  /// \param depth_bias_clamp **[in]** the maximum/minimum value a depth bias
  /// can assume
  /// \param depth_bias_slope_factor **[in]** value added to fragment's slope in
  /// depth bias calculations
  /// \param line_width **[in]** line width in rendered lines
  /// \param rasterization_state_create_info **[out]** rasterization state
  /// creation info object
  static void specifyPipelineRasterizationState(
      bool depth_clamp_enable, bool rasterizer_discard_enable,
      VkPolygonMode polygon_mode, VkCullModeFlags culling_mode,
      VkFrontFace front_face, bool depth_bias_enable,
      float depth_bias_constant_factor, float depth_bias_clamp,
      float depth_bias_slope_factor, float line_width,
      VkPipelineRasterizationStateCreateInfo &rasterization_state_create_info);
  /// \brief Specifies a pipeline multisample state
  /// Multisampling allows us to apply anti-aliasing of drawn primitives.
  /// Note: to use per sample shading, the feature sampleRateShading must be
  /// enabled.
  /// \param sample_count **[in]** number of samples generated per pixel
  /// \param per_sample_shading_enable **[in]** trus if per sample shading
  /// should be performed
  /// \param min_sample_shading **[in]** minimum fraction of uniquely shaded
  /// samples
  /// \param sample_masks **[in]** array of bitmasks that controls a fragment's
  /// static coverage
  /// \param alpha_to_coverage_enable **[in]** whether if a fragment's coverage
  /// should be generated based on the fragment's alpha value (true)
  /// \param alpha_to_one_enable **[in]** whether an alpha component of the
  /// fragment's color should be replaced with a 1.0 value
  /// \param multisample_state_create_info **[in]** multisample state creation
  /// info object
  static void specifyPipelineMultisampleState(
      VkSampleCountFlagBits sample_count, bool per_sample_shading_enable,
      float min_sample_shading, VkSampleMask const *sample_masks,
      bool alpha_to_coverage_enable, bool alpha_to_one_enable,
      VkPipelineMultisampleStateCreateInfo &multisample_state_create_info);
  /// \brief Specifies a pipelin depth and stencil state
  /// These stages perform depth and/or stencil test. Depth test discard
  /// fragments that are covered by fragments of closer objects to camera. The
  /// Stencil test performs tests on integer values associated with each
  /// fragment and can serve to various purposes (for example, we can define
  /// arbitrary complex shaped areas of the screen where pixels can be updated).
  /// \param depth_test_enable **[in]** whether depth test is enabled
  /// \param depth_write_enable **[in]** whether if depth values are stored in a
  /// depth buffer
  /// \param depth_compare_op **[in]** how depth values are compared (never,
  /// less, less and equal, equal, greater and equal, greater, not equal,
  /// always)
  /// \param depth_bounds_test_enable **[in]** whether additional depth bounds
  /// tests
  /// \param min_depth_bounds **[in]** minimum depth bounds value
  /// \param max_depth_bounds **[in]** maximum depth bounds value
  /// \param stencil_test_enable **[in]** whether stencil test is enabled
  /// \param front_stencil_test_parameters **[in]** stencil test parameters for
  /// front faces
  /// \param back_stencil_test_parameters **[in]** stencil test parameters for
  /// back faces
  /// \param depth_and_stencil_state_create_info **[out]** depth and stencil
  /// state creation info object
  static void specifyPipelineDepthAndStencilState(
      bool depth_test_enable, bool depth_write_enable,
      VkCompareOp depth_compare_op, bool depth_bounds_test_enable,
      float min_depth_bounds, float max_depth_bounds, bool stencil_test_enable,
      VkStencilOpState front_stencil_test_parameters,
      VkStencilOpState back_stencil_test_parameters,
      VkPipelineDepthStencilStateCreateInfo
          &depth_and_stencil_state_create_info);
  /// \brief Specifies a pipeline blend state
  /// The blend stage mixes the color of a processed fragment with a color that
  /// is already stored in a framebuffer. We can specify which color components
  /// are updated and perform logical operations between the colors. Blending is
  /// controlled separately for each color attachment used during rendering in a
  /// subpass in which a given graphics pipeline is bound.
  /// \param logic_op_enable **[in]** whether a logical operation will be used
  /// \param logic_op **[in]** type of the logical operation
  /// \param attachment_blend_states **[in]** blending parameters for each color
  /// attachement used in the rendering (must be all the same if
  /// independentBlend feature is not enabled)
  /// \param blend_constants **[in]** some blending factors may use a constant
  /// for each color component
  /// \param blend_state_create_info **[out]** blend state creation info object
  static void specifyPipelineBlendState(
      bool logic_op_enable, VkLogicOp logic_op,
      std::vector<VkPipelineColorBlendAttachmentState> const
          &attachment_blend_states,
      std::array<float, 4> const &blend_constants,
      VkPipelineColorBlendStateCreateInfo &blend_state_create_info);
  /// \brief Specifies pipeline dynamic states
  /// Dynamic states allow us to control some of the pipeline's parameters
  /// dynamically by recording specific functions in command buffers.
  /// \param dynamic_states **[in]** list of unique pipeline stages that should
  /// be set dinamically
  /// \param dynamic_state_create_info **[out]** dynamic state creation info
  /// object
  static void specifyPipelineDynamicStates(
      std::vector<VkDynamicState> const &dynamic_states,
      VkPipelineDynamicStateCreateInfo &dynamic_state_create_info);
  /// \brief Create a Pipeline Layout object
  /// Pipeline layouts define what types of resources can be accessed by a given
  /// pipeline. They also define the range of push constants.
  /// \param logical_device **[in]** logical device handle
  /// \param descriptor_set_layouts **[in]** list of descriptor sets through
  /// which resources will be accessed from shaders (same list, with same
  /// indices, as the one binded on the command buffer)
  /// \param push_constant_ranges **[in]** list of unique sets of push constants
  /// used by different shader stages
  /// \param pipeline_layout **[out]** pipeline layout object
  /// \return bool true if success
  static bool createPipelineLayout(
      VkDevice logical_device,
      std::vector<VkDescriptorSetLayout> const &descriptor_set_layouts,
      std::vector<VkPushConstantRange> const &push_constant_ranges,
      VkPipelineLayout &pipeline_layout);
  /// \brief Specifies graphics pipeline creation parameters
  /// \param additional_options **[in]** creation options such as: disable
  /// optimization (faster creation), allow derivatives (allows children
  /// pipelines), derivative (this pipeline is derivated from other pipeline)
  /// \param shader_stage_create_infos **[in]**
  /// \param vertex_input_state_create_info **[in]**
  /// \param input_assembly_state_create_info **[in]**
  /// \param tessellation_state_create_info **[in]**
  /// \param viewport_state_create_info **[in]**
  /// \param rasterization_state_create_info **[in]**
  /// \param multisample_state_create_info **[in]**
  /// \param depth_and_stencil_state_create_info **[in]**
  /// \param blend_state_create_info **[in]**
  /// \param dynamic_state_creat_info **[in]**
  /// \param pipeline_layout **[in]**
  /// \param render_pass **[in]** render pass handle in which this pipeline will
  /// perform on
  /// \param subpass **[in]** index of the render pass's subpass
  /// \param base_pipeline_handle **[in]** the parent pipeline (in case this
  /// pipeline will be a derivated one)
  /// \param base_pipeline_index **[in]** the parent pipeline batch index (in
  /// case the parent pipeline belongs to the same batch of pipelines), provide
  /// -1 otherwise
  /// \param graphics_pipeline_create_info **[out]** graphics pipeline creation
  /// info object
  static void specifyGraphicsPipelineCreationParameters(
      VkPipelineCreateFlags additional_options,
      std::vector<VkPipelineShaderStageCreateInfo> const
          &shader_stage_create_infos,
      VkPipelineVertexInputStateCreateInfo const
          &vertex_input_state_create_info,
      VkPipelineInputAssemblyStateCreateInfo const
          &input_assembly_state_create_info,
      VkPipelineTessellationStateCreateInfo const
          *tessellation_state_create_info,
      VkPipelineViewportStateCreateInfo const *viewport_state_create_info,
      VkPipelineRasterizationStateCreateInfo const
          &rasterization_state_create_info,
      VkPipelineMultisampleStateCreateInfo const *multisample_state_create_info,
      VkPipelineDepthStencilStateCreateInfo const
          *depth_and_stencil_state_create_info,
      VkPipelineColorBlendStateCreateInfo const *blend_state_create_info,
      VkPipelineDynamicStateCreateInfo const *dynamic_state_creat_info,
      VkPipelineLayout pipeline_layout, VkRenderPass render_pass,
      uint32_t subpass, VkPipeline base_pipeline_handle,
      int32_t base_pipeline_index,
      VkGraphicsPipelineCreateInfo &graphics_pipeline_create_info);
  /// \brief Create a Pipeline Cache Object
  /// Creating a pipeline object can be very time consuming on execution. A
  /// pipeline cache can be created to accelerate this process. Usually, the
  /// pipeline cache is saved in a file and loaded by the application when its
  /// is run again.
  /// \param logical_device **[in]** logical device handle
  /// \param cache_data **[in]** cache data, if available from other caches
  /// \param pipeline_cache **[out]** pipeline cache object
  /// \return bool true if success
  static bool
  createPipelineCacheObject(VkDevice logical_device,
                            std::vector<unsigned char> const &cache_data,
                            VkPipelineCache &pipeline_cache);
  /// \brief Retrieves data from a pipeline cache
  /// \param logical_device **[in]** logical device handle
  /// \param pipeline_cache **[in]** pipeline cache from which data should be
  /// retrieved
  /// \param pipeline_cache_data **[out]** pipeline cache data
  /// \return bool true if success
  static bool retrieveDataFromPipelineCache(
      VkDevice logical_device, VkPipelineCache pipeline_cache,
      std::vector<unsigned char> &pipeline_cache_data);
  /// \brief Merges multiple pipeline cache objects
  /// \param logical_device **[in]** logical device handle
  /// \param target_pipeline_cache **[in]**
  /// \param source_pipeline_caches **[in]**
  /// \return bool true if success
  static bool mergeMultiplePipelineCacheObjects(
      VkDevice logical_device, VkPipelineCache target_pipeline_cache,
      std::vector<VkPipelineCache> const &source_pipeline_caches);
  /// \brief Create a Graphics Pipelines object
  /// The graphics pipeline controls how the graphics hardware performs all the
  /// drawing related operations.
  /// \param logical_device **[in]** logical device handle
  /// \param graphics_pipeline_create_infos **[in]**
  /// \param pipeline_cache **[in]** if available, cache can be passed here
  /// \param graphics_pipelines **[out]** graphics pipeline objects
  /// \return bool true if success
  static bool
  createGraphicsPipelines(VkDevice logical_device,
                          std::vector<VkGraphicsPipelineCreateInfo> const
                              &graphics_pipeline_create_infos,
                          VkPipelineCache pipeline_cache,
                          std::vector<VkPipeline> &graphics_pipelines);
  /// \brief Create a Compute Pipeline object
  /// Compute pipelines are used to perform mathematical operations and cannot
  /// be used inside render passes. The compute shader doesn't have any input or
  /// output variables, only uniform variables.
  /// \param logical_device **[in]** logical device handle
  /// \param additional_options **[in]** creation options such as: disable
  /// optimization (faster creation), allow derivatives (allows children
  /// pipelines), derivative (this pipeline is derivated from other pipeline)
  /// \param compute_shader_stage **[in]**
  /// \param pipeline_layout **[in]**
  /// \param base_pipeline_handle **[in]** the parent pipeline (in case this
  /// pipeline will be a derivated one)
  /// \param pipeline_cache **[in]**
  /// \param compute_pipeline **[out]** compute pipeline object
  /// \return bool true if success
  static bool createComputePipeline(
      VkDevice logical_device, VkPipelineCreateFlags additional_options,
      VkPipelineShaderStageCreateInfo const &compute_shader_stage,
      VkPipelineLayout pipeline_layout, VkPipeline base_pipeline_handle,
      VkPipelineCache pipeline_cache, VkPipeline &compute_pipeline);
  /// \brief Binds a pipeline object
  /// Note: If it is a graphics pipeline, make sure to bind it after the
  /// recording of a render pass. If it is a compute pipeline, make sure there
  /// is no render passes in the command buffer.
  /// \param command_buffer **[in]** command buffer in recording state
  /// \param pipeline_type **[in]**
  /// \param pipeline **[in]**
  static void bindPipelineObject(VkCommandBuffer command_buffer,
                                 VkPipelineBindPoint pipeline_type,
                                 VkPipeline pipeline);
  /// Note: make sure all commands that use the pipeline are finished. A fence
  /// can be used in this situation.
  /// \param logical_device **[in]** logical device handle
  /// \param pipeline **[in/out]** receives VK_NULL_HANDLE
  static void destroyPipeline(VkDevice logical_device, VkPipeline &pipeline);
  /// \param logical_device **[in]** logical device handle
  /// \param pipeline_cache **[in/out]** receives VK_NULL_HANDLE
  static void destroyPipelineCache(VkDevice logical_device,
                                   VkPipelineCache &pipeline_cache);
  /// Note: If it is being used to bind descriptor sets or update push
  /// constants, be sure to wait hardware to process all command buffers that
  /// use it
  /// \param logical_device **[in]** logical device handle
  /// \param pipeline_layout **[in/out]** receives VK_NULL_HANDLE
  static void destroyPipelineLayout(VkDevice logical_device,
                                    VkPipelineLayout &pipeline_layout);
  /// \brief
  /// Note: Shader modules are used only on pipeline creation, they can be
  /// destroyed as soon the pipeline is created.
  /// \param logical_device **[in]** logical device handle
  /// \param shader_module **[in/out]** receives VK_NULL_HANDLE
  static void destroyShaderModule(VkDevice logical_device,
                                  VkShaderModule &shader_module);

  // VULKAN COMMANDS OF A COMMAND BUFFER
  // -----------------------------------
  // Here is a list of auxiliary functions to additional commands that can be
  // recorded in a command buffer.

  /// \brief Clears a image with a specified color
  /// Note: this command cannot be called inside a render pass. Only images with
  /// color aspect and a color format can be cleared.
  /// \param command_buffer **[in]**
  /// \param image **[in]**
  /// \param image_layout **[in]**
  /// \param image_subresource_ranges **[in]** mipmap levels and array layers
  /// that should also cleared
  /// \param clear_color **[in]**
  static void clearColorImage(
      VkCommandBuffer command_buffer, VkImage image, VkImageLayout image_layout,
      std::vector<VkImageSubresourceRange> const &image_subresource_ranges,
      VkClearColorValue &clear_color);
  /// Note: this command can only be applied to images created with a transfer
  /// dst usage, since clearing is considered a transfer operation
  /// \param command_buffer **[in]**
  /// \param image **[in]**
  /// \param image_layout **[in]**
  /// \param image_subresource_ranges **[in]**
  /// \param clear_value **[in]**
  static void clearDepthStencilImage(
      VkCommandBuffer command_buffer, VkImage image, VkImageLayout image_layout,
      std::vector<VkImageSubresourceRange> const &image_subresource_ranges,
      VkClearDepthStencilValue &clear_value);
  /// \brief Clears multiple regions of given attachments
  /// Note: this command can be called inside render passes
  /// \param command_buffer **[in]**
  /// \param attachments **[in]** list of attachments
  /// \param rects **[in]** list of regions
  static void
  clearRenderPassAttachments(VkCommandBuffer command_buffer,
                             std::vector<VkClearAttachment> const &attachments,
                             std::vector<VkClearRect> const &rects);
  /// \param command_buffer **[in]**
  /// \param first_binding **[in]**
  /// \param buffers_parameters **[in]**
  static void bindVertexBuffers(
      VkCommandBuffer command_buffer, uint32_t first_binding,
      std::vector<VertexBufferParameters> const &buffers_parameters);
  /// \param command_buffer **[in]**
  /// \param buffer **[in]**
  /// \param memory_offset **[in]**
  /// \param index_type **[in]**
  static void bindIndexBuffer(VkCommandBuffer command_buffer, VkBuffer buffer,
                              VkDeviceSize memory_offset,
                              VkIndexType index_type);
  /// \param command_buffer **[in]**
  /// \param pipeline_layout **[in]**
  /// \param pipeline_stages **[in]**
  /// \param offset **[in]**
  /// \param size **[in]**
  /// \param data **[in]**
  static void provideDataToShadersThroughPushConstants(
      VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout,
      VkShaderStageFlags pipeline_stages, uint32_t offset, uint32_t size,
      void *data);
  /// \param command_buffer **[in]**
  /// \param first_viewport **[in]**
  /// \param viewports **[in]**
  static void
  setViewportStateDynamically(VkCommandBuffer command_buffer,
                              uint32_t first_viewport,
                              std::vector<VkViewport> const &viewports);
  /// \param command_buffer **[in]**
  /// \param first_scissor **[in]**
  /// \param scissors **[in]**
  static void setScissorStateDynamically(VkCommandBuffer command_buffer,
                                         uint32_t first_scissor,
                                         std::vector<VkRect2D> const &scissors);
  /// \param command_buffer **[in]**
  /// \param line_width **[in]**
  static void setLineWidthStateDynamically(VkCommandBuffer command_buffer,
                                           float line_width);
  /// \param command_buffer **[in]**
  /// \param constant_factor **[in]**
  /// \param clamp **[in]**
  /// \param slope_factor **[in]**
  static void setDepthBiasStateDynamically(VkCommandBuffer command_buffer,
                                           float constant_factor, float clamp,
                                           float slope_factor);
  /// \param command_buffer **[in]**
  /// \param blend_constants **[in]**
  void setBlendConstantsStateDynamically(
      VkCommandBuffer command_buffer,
      std::array<float, 4> const &blend_constants);
  /// \param command_buffer **[in]**
  /// \param vertex_count **[in]**
  /// \param instance_count **[in]**
  /// \param first_vertex **[in]**
  /// \param first_instance **[in]**
  void drawGeometry(VkCommandBuffer command_buffer, uint32_t vertex_count,
                    uint32_t instance_count, uint32_t first_vertex,
                    uint32_t first_instance);
  /// \param command_buffer **[in]**
  /// \param index_count **[in]**
  /// \param instance_count **[in]**
  /// \param first_index **[in]**
  /// \param vertex_offset **[in]**
  /// \param first_instance **[in]**
  void drawIndexedGeometry(VkCommandBuffer command_buffer, uint32_t index_count,
                           uint32_t instance_count, uint32_t first_index,
                           uint32_t vertex_offset, uint32_t first_instance);
  /// \param command_buffer **[in]**
  /// \param x_size **[in]**
  /// \param y_size **[in]**
  /// \param z_size **[in]**
  void dispatchComputeWork(VkCommandBuffer command_buffer, uint32_t x_size,
                           uint32_t y_size, uint32_t z_size);
  /// \param command_buffer **[in]**
  /// \param secondary_command_buffers **[in]**
  void executeSecondaryCommandBufferInsidePrimaryCommandBuffer(
      VkCommandBuffer command_buffer,
      std::vector<VkCommandBuffer> const &secondary_command_buffers);
  /// \param threads_parameters **[in]**
  /// \param queue **[in]**
  /// \param wait_semaphore_infos **[in]**
  /// \param signal_semaphores **[in]**
  /// \param fence **[in]**
  /// \return bool
  // bool recordCommandBuffersOnMultipleThreads(
  //  std::vector<CommandBufferRecordingThreadParameters> const
  //    &threads_parameters,
  // VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos,
  // std::vector<VkSemaphore> signal_semaphores, VkFence fence);

  // EXAMPLES
  // --------
  // Here follows some auxiliary methods for instance and device creation

  /// Same as **createVulkanInstance**, but automatically appends the
  /// VK_KHR_***_SURFACE_EXTANTION_NAME to the desired extensions list
  /// \param desired_extensions **[in]** list of desired extensions
  /// \param application_name **[in]** application name
  /// \param instance **[out]** instance handle
  /// \return bool true if success
  static bool createVulkanInstanceWithWsiExtensionsEnabled(
      std::vector<char const *> &desired_extensions,
      char const *const application_name, VkInstance &instance);
  /// Same as **createLogicalDevice**, but automatically appends the
  /// VK_KHR_SWAPCHAIN_EXTENTSION_NAME to the desired extensions list
  /// \param physical_device **[in]** physical device handle
  /// \param queue_infos **[in]** queues description
  /// \param desired_extensions **[in]** desired extensions
  /// \param desired_features **[in]** desired features
  /// \param logical_device **[out]** logical device handle
  /// \return bool true if success
  static bool createLogicalDeviceWithWsiExtensionsEnabled(
      VkPhysicalDevice physical_device,
      std::vector<QueueFamilyInfo> queue_infos,
      std::vector<char const *> &desired_extensions,
      VkPhysicalDeviceFeatures *desired_features, VkDevice &logical_device);
  static bool createLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(
      VkInstance instance, VkDevice &logical_device, VkQueue &graphics_queue,
      VkQueue &compute_queue);
  static bool createSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(
      VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface,
      VkDevice logical_device, VkImageUsageFlags swapchain_image_usage,
      VkExtent2D &image_size, VkFormat &image_format,
      VkSwapchainKHR &old_swapchain, VkSwapchainKHR &swapchain,
      std::vector<VkImage> &swapchain_images);
  static bool synchronizeTwoCommandBuffers(
      VkQueue first_queue,
      std::vector<WaitSemaphoreInfo> first_wait_semaphore_infos,
      std::vector<VkCommandBuffer> first_command_buffers,
      std::vector<WaitSemaphoreInfo> synchronizing_semaphores,
      VkQueue second_queue, std::vector<VkCommandBuffer> second_command_buffers,
      std::vector<VkSemaphore> second_signal_semaphores, VkFence second_fence);
  static bool checkIfProcessingOfSubmittedCommandBufferHasFinished(
      VkDevice logical_device, VkQueue queue,
      std::vector<WaitSemaphoreInfo> wait_semaphore_infos,
      std::vector<VkCommandBuffer> command_buffers,
      std::vector<VkSemaphore> signal_semaphores, VkFence fence,
      uint64_t timeout, VkResult &wait_status);

private:
  // VulkanLibraryType vulkanLibrary_;
};

} // namespace circe

#endif // CIRCE_VULKAN_LIBRARY
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
/// \file vk_image.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-09-08
///
/// \brief

#ifndef CIRCE_VK_IMAGE_H
#define CIRCE_VK_IMAGE_H

#include <circe/vk/core/logical_device.h>

namespace circe::vk {

/// \brief Holds a vulkan image object
/// Images represent data that can have multiple dimensions. They also have
/// mipmap levels and layers. Each texel can have multiple samples. Images
/// can serve as source of data for copy operations and work similar to OpenGL
/// textures.
/// \note This class uses RAII
class Image final {
public:
  /// An image view is a collection of properties and a reference to the image
  /// resource. It allows all or part of an existing image to be seen as a
  /// different format.
  /// \note This class uses RAII
  class View {
    friend class Image;
    friend class Ref;
  public:
    // ***********************************************************************
    //                           CONSTRUCTORS
    // ***********************************************************************
    View() = default;
    View(const View &&other) = delete;
    View(View &&other) noexcept;
    ~View();
    // ***********************************************************************
    //                           OPERATORS
    // ***********************************************************************
    View &operator=(const View &other) = delete;
    View &operator=(View &&other) noexcept;
    // ***********************************************************************
    //                           FIELDS
    // ***********************************************************************
    void destroy();
    [[nodiscard]] bool good() const;
    [[nodiscard]] VkImageView handle() const;

  private:
    /// Creates a image view of a portion of the given image
    /// \param view_type **[in]**
    /// \param format **[in]** data format
    /// \param aspect **[in]** context: color, depth or stencil
    /// \param image_view **[out]** image view object
    /// \return bool true if success
    View(VkDevice vk_device, VkImage vk_image,
         VkImageViewType view_type, VkFormat format,
         VkImageAspectFlags aspect);
    ///
    /// \param image
    /// \param view_type
    /// \param format
    /// \param aspect
    View(const Image* image,
         VkImageViewType view_type, VkFormat format,
         VkImageAspectFlags aspect);


    VkImage vk_image_{VK_NULL_HANDLE};
    VkDevice vk_device_{VK_NULL_HANDLE};
    VkImageView vk_image_view_{VK_NULL_HANDLE};
  };
  /// Holds a image reference containing the image handle and the respective
  /// logical device handle
  class Ref {
    friend class Image;
  public:
    /// \param logical_device
    /// \param vk_image_handle
    Ref(const LogicalDevice::Ref &logical_device, VkImage vk_image_handle);
    ~Ref();
    [[nodiscard]] View view(VkImageViewType view_type, VkFormat format,
                            VkImageAspectFlags aspect) const;
    [[nodiscard]] VkImage handle() const;
    [[maybe_unused]] [[nodiscard]] VkDevice deviceHandle() const;
  private:
    explicit Ref(const Image *image);

    // TODO: what is best? store a Image pointer instead?
    VkDevice vk_device_{VK_NULL_HANDLE};
    VkImage vk_image_{VK_NULL_HANDLE};
    VkFormat format_{};
    VkExtent3D size_{};
    u32 mip_levels_{1};
  };
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Image();
  // VK_IMAGE_USAGE_TRANSFER_SRC_BIT specifies that the image can be used as
  // a source of data for copy operations VK_IMAGE_USAGE_TRANSFER_DST_BIT
  // specifies that we can copy data to the image VK_IMAGE_USAGE_SAMPLED_BIT
  // indicates that we can sample data from the image inside shaders
  // VK_IMAGE_USAGE_STORAGE_BIT specifies that the image can be used as a
  // storage image inside shaders
  // VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT specifies that we can render into
  // an image (use it as a color render target/attachment in a framebuffer)
  // VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT indicates that the image
  // can be used as a depth and/or stencil buffer (as a depth render
  // target/attachment in a framebuffer)
  // VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT indicates that the memory bound
  // to the image will be allocated lazily (on demand)
  // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT specifies that the image can be
  // used as an input attachment inside shaders

  /// \param logical_device **[in]** logical device (on which the image
  /// will be created)
  /// \param type **[in]** number of dimensions of the image
  /// \param format **[in]** number of components and number of bits of a
  /// texel in the image
  /// \param size **[in]** image's dimensions (in texels)
  /// \param num_mipmaps **[in]** number of mipmap levels
  /// \param num_layers **[in]** number of layers
  /// \param samples **[in]** number of samples
  /// \param usage_scenarios **[in]**
  /// \param cubemap **[in]**
  Image(const LogicalDevice::Ref &logical_device, VkImageType type, VkFormat format,
        VkExtent3D size, u32 num_mipmaps, u32 num_layers,
        VkSampleCountFlagBits samples, VkImageUsageFlags usage_scenarios,
        bool cubemap);
  Image(const Image &other) = delete;
  Image(Image &&other) noexcept;
  ~Image();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Image &operator=(const Image &other) = delete;
  Image &operator=(Image &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  /// \param logical_device **[in]** logical device (on which the image
  /// will be created)
  /// \param type **[in]** number of dimensions of the image
  /// \param format **[in]** number of components and number of bits of a
  /// texel in the image
  /// \param size **[in]** image's dimensions (in texels)
  /// \param num_mipmaps **[in]** number of mipmap levels
  /// \param num_layers **[in]** number of layers
  /// \param samples **[in]** number of samples
  /// \param usage_scenarios **[in]**
  /// \param cubemap **[in]**
  bool init(const LogicalDevice::Ref &logical_device, VkImageType type, VkFormat format,
            VkExtent3D size, u32 num_mipmaps, u32 num_layers,
            VkSampleCountFlagBits samples, VkImageUsageFlags usage_scenarios,
            bool cubemap);
  ///
  void destroy();
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  ///\return const LogicalDevice& device owner of its resource
  [[nodiscard]] const LogicalDevice::Ref &device() const;
  ///\return VkImage vulkan handle object
  [[nodiscard]] VkImage handle() const;
  ///\return u32 number of mipmap levels
  [[nodiscard]] u32 mipLevels() const;
  ///\return VkExtent3D image dimensions (in texels)
  [[nodiscard]] VkExtent3D size() const;
  ///\return VkFormat
  [[nodiscard]] VkFormat format() const;
  // ***********************************************************************
  //                           QUERIES
  // ***********************************************************************
  ///\return bool
  [[nodiscard]] bool good() const;
  ///\brief Retrieves information about a given subresource in the image.
  /// Images subresources are mipmap levels, depth or stencil components, array
  /// layers.
  ///\param aspect_mask **[in]** one or a combination of
  /// VK_IMAGE_ASPECT_[COLOR|DEPTH|STENCIL]_BIT
  /// ex : VK_IMAGE_ASPECT_COLOR_BIT for color images
  ///\param mip_level **[in]** the mipmap level for which the parameters
  /// are to be returned.
  ///\param array_layer **[in]** generally 0, as parameters are not expect
  /// to change across layers.
  ///\param subresource_layout **[out]**
  ///\return bool true if success
  bool subresourceLayout(VkImageAspectFlags aspect_mask, u32 mip_level,
                         u32 array_layer, VkSubresourceLayout &subresource_layout) const;
  ///\brief Information about the type of memory and how much of it the image
  /// resource requires.
  ///\param memory_requirements **[out]**
  ///\return bool true if success
  bool memoryRequirements(VkMemoryRequirements &memory_requirements) const;
  ///
  /// \param view_type
  /// \param format
  /// \param aspect
  /// \return
  [[nodiscard]] View view(VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect) const;

  Image::Ref ref() const;

private:
  LogicalDevice::Ref logical_device_;
  VkImage vk_image_ = VK_NULL_HANDLE;
  VkFormat format_{};
  VkExtent3D size_{};
  u32 mip_levels_{1};
};


} // namespace circe::vk

#endif
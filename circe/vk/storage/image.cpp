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
/// \file vk_image.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-09-08
///
/// \brief

#include <circe/vk/storage/image.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

Image::Ref::Ref(const Image *image) {
  vk_device_ = image->device().handle();
  vk_image_ = image->handle();
  format_ = image->format_;
  size_ = image->size_;
  mip_levels_ = image->mip_levels_;
}

Image::Ref::Ref(const LogicalDevice::Ref &logical_device, VkImage vk_image_handle) :
    vk_device_{logical_device.handle()}, vk_image_{vk_image_handle} {}

Image::Ref::~Ref() = default;

VkImage Image::Ref::handle() const {
  return vk_image_;
}
[[maybe_unused]] VkDevice Image::Ref::deviceHandle() const {
  return vk_device_;
}

Image::View Image::Ref::view(VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect) const {
  return Image::View(vk_device_, vk_image_, view_type, format, aspect);
}

Image::View::View(VkDevice vk_device, VkImage vk_image, VkImageViewType view_type,
                  VkFormat format, VkImageAspectFlags aspect)
    : vk_image_(vk_image), vk_device_{vk_device} {
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_image_, "using bad image.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_device_, "using bad device.")
  VkImageViewCreateInfo image_view_create_info = {
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType sType
      nullptr,         // const void               * pNext
      0,               // VkImageViewCreateFlags     flags
      vk_image_, // VkImage                    image
      view_type,       // VkImageViewType            viewType
      format,          // VkFormat                   format
      {
          // VkComponentMapping         components
          VK_COMPONENT_SWIZZLE_IDENTITY, // VkComponentSwizzle         r
          VK_COMPONENT_SWIZZLE_IDENTITY, // VkComponentSwizzle         g
          VK_COMPONENT_SWIZZLE_IDENTITY, // VkComponentSwizzle         b
          VK_COMPONENT_SWIZZLE_IDENTITY  // VkComponentSwizzle         a
      },
      {
          // VkImageSubresourceRange    subresourceRange
          aspect,             // VkImageAspectFlags         aspectMask
          0,                  // u32                   baseMipLevel
          1, // u32 levelCount VK_REMAINING_MIP_LEVELS
          0,                  // u32                   baseArrayLayer
          VK_REMAINING_ARRAY_LAYERS // u32                   layerCount
      }};

  CHECK_VULKAN(vkCreateImageView(vk_device_,
                                 &image_view_create_info, nullptr,
                                 &vk_image_view_))
}

Image::View::View(const Image *image, VkImageViewType view_type,
                  VkFormat format, VkImageAspectFlags aspect)
    : vk_image_(image->handle()), vk_device_{image->device().handle()} {
  HERMES_VALIDATE_EXP_WITH_WARNING(image->good(), "using bad image.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_device_, "using bad device.")
  VkImageViewCreateInfo image_view_create_info = {
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType sType
      nullptr,         // const void               * pNext
      0,               // VkImageViewCreateFlags     flags
      vk_image_, // VkImage                    image
      view_type,       // VkImageViewType            viewType
      format,          // VkFormat                   format
      {
          // VkComponentMapping         components
          VK_COMPONENT_SWIZZLE_IDENTITY, // VkComponentSwizzle         r
          VK_COMPONENT_SWIZZLE_IDENTITY, // VkComponentSwizzle         g
          VK_COMPONENT_SWIZZLE_IDENTITY, // VkComponentSwizzle         b
          VK_COMPONENT_SWIZZLE_IDENTITY  // VkComponentSwizzle         a
      },
      {
          // VkImageSubresourceRange    subresourceRange
          aspect,             // VkImageAspectFlags         aspectMask
          0,                  // u32                   baseMipLevel
          image->mipLevels(), // u32 levelCount VK_REMAINING_MIP_LEVELS
          0,                  // u32                   baseArrayLayer
          VK_REMAINING_ARRAY_LAYERS // u32                   layerCount
      }};

  CHECK_VULKAN(vkCreateImageView(vk_device_,
                                 &image_view_create_info, nullptr,
                                 &vk_image_view_))
}

Image::View::View(Image::View &&other) noexcept: vk_image_(other.vk_image_),
                                                 vk_device_{other.vk_device_} {
  vk_image_view_ = other.vk_image_view_;
  other.vk_image_view_ = VK_NULL_HANDLE;
}

Image::View::~View() {
  destroy();
}

Image::View &Image::View::operator=(View &&other) noexcept {
  vk_device_ = other.vk_device_;
  vk_image_view_ = other.vk_image_view_;
  other.vk_image_view_ = VK_NULL_HANDLE;
  return *this;
}

void Image::View::destroy() {
  if (vk_device_ && vk_image_ && vk_image_view_)
    vkDestroyImageView(vk_device_, vk_image_view_, nullptr);
  vk_image_view_ = VK_NULL_HANDLE;
}

bool Image::View::good() const {
  return vk_image_ != VK_NULL_HANDLE && vk_image_view_ != VK_NULL_HANDLE;
}

VkImageView Image::View::handle() const { return vk_image_view_; }

Image::Image() = default;

Image::Image(const LogicalDevice::Ref &logical_device, VkImageType type,
             VkFormat format, VkExtent3D size, u32 num_mipmaps,
             u32 num_layers, VkSampleCountFlagBits samples,
             VkImageUsageFlags usage_scenarios, bool cubemap) {
  init(logical_device, type, format, size, num_mipmaps, num_layers, samples, usage_scenarios, cubemap);
}

Image::Image(Image &&other) noexcept {
  *this = std::move(other);
}

Image::~Image() {
  destroy();
}

Image &Image::operator=(Image &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  vk_image_ = other.vk_image_;
  other.vk_image_ = VK_NULL_HANDLE;
  format_ = other.format_;
  size_ = other.size_;
  mip_levels_ = other.mip_levels_;
  return *this;
}

bool Image::init(const LogicalDevice::Ref &logical_device,
                 VkImageType type,
                 VkFormat format,
                 VkExtent3D size,
                 u32 num_mipmaps,
                 u32 num_layers,
                 VkSampleCountFlagBits samples,
                 VkImageUsageFlags usage_scenarios,
                 bool cubemap) {
  destroy();


  logical_device_ = logical_device;
  format_ = format;
  size_ = size;
  mip_levels_ = num_mipmaps;

  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")

  VkImageCreateInfo image_create_info = {
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, // VkStructureType          sType
      nullptr,                             // const void             * pNext
      cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
              : 0u, // VkImageCreateFlags       flags
      type,         // VkImageType              imageType
      format,       // VkFormat                 format
      size,         // VkExtent3D               extent
      mip_levels_,  // u32                 mipLevels
      cubemap ? 6 * num_layers : num_layers, // u32 arrayLayers
      samples,                               // VkSampleCountFlagBits    samples
      VK_IMAGE_TILING_OPTIMAL,               // VkImageTiling            tiling
      usage_scenarios,                       // VkImageUsageFlags        usage
      VK_SHARING_MODE_EXCLUSIVE, // VkSharingMode            sharingMode
      0,       // u32                 queueFamilyIndexCount
      nullptr, // const u32         * pQueueFamilyIndices
      VK_IMAGE_LAYOUT_UNDEFINED // VkImageLayout            initialLayout
  };
  CHECK_VULKAN(vkCreateImage(logical_device.handle(), &image_create_info,
                             nullptr, &vk_image_))
  HERMES_LOG_AND_RETURN_VALUE_IF_NOT(vk_image_, false, "Could not create image.")
  return true;
}

void Image::destroy() {
  if (logical_device_.good() && vk_image_)
    vkDestroyImage(logical_device_.handle(), vk_image_, nullptr);
  vk_image_ = VK_NULL_HANDLE;
}

VkImage Image::handle() const { return vk_image_; }

const LogicalDevice::Ref &Image::device() const { return logical_device_; }

bool Image::good() const { return vk_image_ != VK_NULL_HANDLE; }

bool Image::subresourceLayout(VkImageAspectFlags aspect_mask,
                              u32 mip_level, u32 array_layer,
                              VkSubresourceLayout &subresource_layout) const {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_image_, "using bad image.")
  VkImageSubresource subresource{};
  subresource.aspectMask = aspect_mask;
  subresource.mipLevel = mip_level;
  subresource.arrayLayer = array_layer;
  vkGetImageSubresourceLayout(logical_device_.handle(), vk_image_,
                              &subresource, &subresource_layout);
  return true;
}

bool Image::memoryRequirements(
    VkMemoryRequirements &memory_requirements) const {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_image_, "using bad image.")
  vkGetImageMemoryRequirements(logical_device_.handle(), vk_image_,
                               &memory_requirements);
  return true;
}

u32 Image::mipLevels() const { return mip_levels_; }

VkExtent3D Image::size() const { return size_; }

VkFormat Image::format() const { return format_; }

Image::View Image::view(VkImageViewType view_type, VkFormat v_format, VkImageAspectFlags aspect) const {
  return Image::View(logical_device_.handle(), vk_image_, view_type, v_format, aspect);
}

Image::Ref Image::ref() const {
  return Image::Ref(this);
}

} // namespace circe::vk
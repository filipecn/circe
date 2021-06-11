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
/// \file vk_texture_image.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-12-09
///
/// \brief

#include <circe/vk/texture/texture.h>
#include <circe/vk/utils/vk_debug.h>
#include <circe/vk/pipeline/command_buffer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace circe::vk {

Texture::Texture() = default;

Texture::Texture(const LogicalDevice::Ref &logical_device,
                 const std::string &filename, uint32_t queue_family_index,
                 VkQueue queue)
    : logical_device_(logical_device) {
  auto tex_image_format = VK_FORMAT_R8G8B8A8_SRGB;
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load(filename.c_str(), &tex_width, &tex_height,
                              &tex_channels, STBI_rgb_alpha);
  if (!pixels) {
    PONOS_LOG_WARNING("could not load texture image file!")
    return;
  }
  uint32_t
      mip_levels = static_cast<uint32_t>(std::floor(std::log2((tex_width > tex_height) ? tex_width : tex_height))) + 1;
  VkDeviceSize image_size = tex_width * tex_height * 4;

  Buffer staging_buffer(logical_device_, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  DeviceMemory staging_buffer_memory(staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  staging_buffer_memory.bind(staging_buffer);
  staging_buffer_memory.copy(pixels, staging_buffer.size());
  stbi_image_free(pixels);
  // Allocate image data on device
  VkExtent3D size = {};
  size.width = tex_width;
  size.height = tex_height;
  size.depth = 1;
  image_.init(logical_device_, VK_IMAGE_TYPE_2D, tex_image_format, size, mip_levels, 1,
              VK_SAMPLE_COUNT_1_BIT,
              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                  VK_IMAGE_USAGE_SAMPLED_BIT, false);
  image_memory_ = DeviceMemory(image_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  image_memory_.bind(image_);
  // copy data to device
  CommandPool::submitCommandBuffer(
      logical_device_, queue_family_index, queue, [&](CommandBuffer &cb) {
        ImageMemoryBarrier barrier(image_, VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cb.transitionImageLayout(barrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT);
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {size.width, size.height, 1};
        cb.copy(staging_buffer, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {region});
        // ImageMemoryBarrier after_barrier(
        //     *image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        // cb.transitionImageLayout(after_barrier,
        // VK_PIPELINE_STAGE_TRANSFER_BIT,
        //                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      });
  generateMipmaps(queue, queue_family_index);
}

Texture::Texture(const LogicalDevice::Ref &logical_device, VkImageType type,
                 VkFormat format, VkExtent3D size, uint32_t num_mipmaps,
                 uint32_t num_layers, VkSampleCountFlagBits samples,
                 VkImageUsageFlags usage_scenarios, bool cubemap)
    : logical_device_(logical_device) {
  image_.init(logical_device_, type, format, size, num_mipmaps,
              num_layers, samples, usage_scenarios, cubemap);
}

Texture::Texture(Texture &&other) noexcept {
  *this = std::move(other);
}

Texture::~Texture() {
  destroy();

}

Texture &Texture::operator=(Texture &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  image_ = std::move(other.image_);
  image_memory_ = std::move(other.image_memory_);
  return *this;
}

void Texture::destroy() {
  image_.destroy();
  image_memory_.destroy();
}

bool Texture::good() const {
  return logical_device_.good() && image_.good() && image_memory_.good();
}

void Texture::setData(const unsigned char *data, uint32_t queue_family_index,
                      VkQueue queue) {
  VkDeviceSize image_size = image_.size().width * image_.size().height * 4;
  Buffer staging_buffer(logical_device_, image_size,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  DeviceMemory staging_buffer_memory(staging_buffer,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  staging_buffer_memory.bind(staging_buffer);
  staging_buffer_memory.copy(data, staging_buffer.size());
  image_memory_ = DeviceMemory(image_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  image_memory_.bind(image_);
  // copy data to device
  CommandPool::submitCommandBuffer(
      logical_device_, queue_family_index, queue, [&](CommandBuffer &cb) {
        ImageMemoryBarrier barrier(image_, VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cb.transitionImageLayout(barrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT);
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = image_.size();
        cb.copy(staging_buffer, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                {region});
        ImageMemoryBarrier after_barrier(
            image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        cb.transitionImageLayout(after_barrier, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      });
}
const Image *Texture::image() const { return &image_; }

void Texture::generateMipmaps(VkQueue queue, uint32_t queue_family_index) {
  // check first if we have support for the blit command:
  VkFormatProperties format_properties;
  logical_device_.physicalDevice().formatProperties(image_.format(), format_properties);
  if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    PONOS_LOG_WARNING("texture image format does not support linear blitting!")
    return;
  }
  // Perform several image transitions (one for each mip level)
  CommandPool::submitCommandBuffer(
      logical_device_, queue_family_index, queue, [&](CommandBuffer &cb) {
        // the same barrier object will be used to
        // all transitions
        ImageMemoryBarrier barrier;
        // first, set common parameters for the barrier
        auto &vk_barrier = barrier.handle();
        vk_barrier.image = image_.handle();
        vk_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vk_barrier.subresourceRange.baseArrayLayer = 0;
        vk_barrier.subresourceRange.layerCount = 1;
        vk_barrier.subresourceRange.levelCount = 1;

        int32_t mip_width = image_.size().width;
        int32_t mip_height = image_.size().height;
        // for each mip level image record the VkCmdBlitImage command
        for (uint32_t i = 1; i < image_.mipLevels(); ++i) {
          vk_barrier.subresourceRange.baseMipLevel = i - 1;
          vk_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
          vk_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
          vk_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
          vk_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
          cb.transitionImageLayout(barrier, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                   VK_PIPELINE_STAGE_TRANSFER_BIT);
          // define blit command data
          VkImageBlit blit = {};
          blit.srcOffsets[0] = {0, 0, 0};
          blit.srcOffsets[1] = {mip_width, mip_height, 1};
          blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          blit.srcSubresource.mipLevel = i - 1;
          blit.srcSubresource.baseArrayLayer = 0;
          blit.srcSubresource.layerCount = 1;
          blit.dstOffsets[0] = {0, 0, 0};
          blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1,
                                mip_height > 1 ? mip_height / 2 : 1, 1};
          blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          blit.dstSubresource.mipLevel = i;
          blit.dstSubresource.baseArrayLayer = 0;
          blit.dstSubresource.layerCount = 1;
          // record command
          cb.blit(image_, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image_,
                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {blit},
                  VK_FILTER_LINEAR);
          // now transition the mip level i-1
          vk_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
          vk_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
          vk_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
          vk_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
          // This transition waits on the current blit command to finish
          // All sampling operations will wait on this transition to finish.
          cb.transitionImageLayout(barrier, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
          if (mip_width > 1)
            mip_width /= 2;
          if (mip_height > 1)
            mip_height /= 2;
        }
        // Now, transition the last mip level
        vk_barrier.subresourceRange.baseMipLevel = image_.mipLevels() - 1;
        vk_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        vk_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        vk_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vk_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        cb.transitionImageLayout(barrier, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      });
}

} // namespace circe::vk

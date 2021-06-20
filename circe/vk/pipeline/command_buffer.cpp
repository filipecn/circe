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
///\file vk_command_buffer.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-10-25
///
///\brief

#include <circe/vk/pipeline/command_buffer.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

RenderPassBeginInfo::RenderPassBeginInfo(const RenderPass::Ref &renderpass,
                                         const Framebuffer &framebuffer) {
  PONOS_VALIDATE_EXP_WITH_WARNING(renderpass.good(), "using bad renderpass.")
  PONOS_VALIDATE_EXP_WITH_WARNING(framebuffer.good(), "using bad framebuffer.")
  info_.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info_.pNext = nullptr;
  info_.renderPass = renderpass.handle();
  info_.framebuffer = framebuffer.handle();
  info_.renderArea.offset.x = info_.renderArea.offset.y = 0;
  info_.renderArea.extent.width = framebuffer.size().width;
  info_.renderArea.extent.height = framebuffer.size().height;
  info_.clearValueCount = 0;
  info_.pClearValues = nullptr;
}

void RenderPassBeginInfo::setRenderArea(int32_t x, int32_t y, u32 width,
                                        u32 height) {
  info_.renderArea.offset.x = x;
  info_.renderArea.offset.y = y;
  info_.renderArea.extent.width = width;
  info_.renderArea.extent.height = height;
}

void RenderPassBeginInfo::addClearColorValuef(float r, float g, float b,
                                              float a) {
  VkClearValue v;
  v.color.float32[0] = r;
  v.color.float32[1] = g;
  v.color.float32[2] = b;
  v.color.float32[3] = a;
  clear_values_.emplace_back(v);
  info_.clearValueCount = clear_values_.size();
  info_.pClearValues = clear_values_.data();
}

void RenderPassBeginInfo::addClearColorValuei(int32_t r, int32_t g, int32_t b,
                                              int32_t a) {
  VkClearValue v;
  v.color.int32[0] = r;
  v.color.int32[1] = g;
  v.color.int32[2] = b;
  v.color.int32[3] = a;
  clear_values_.emplace_back(v);
  info_.clearValueCount = clear_values_.size();
  info_.pClearValues = clear_values_.data();
}

void RenderPassBeginInfo::addClearColorValueu(u32 r, u32 g,
                                              u32 b, u32 a) {
  VkClearValue v;
  v.color.uint32[0] = r;
  v.color.uint32[0] = g;
  v.color.uint32[0] = b;
  v.color.uint32[0] = a;
  clear_values_.emplace_back(v);
  info_.clearValueCount = clear_values_.size();
  info_.pClearValues = clear_values_.data();
}

void RenderPassBeginInfo::addClearDepthStencilValue(float depth,
                                                    u32 stencil) {
  VkClearValue v;
  v.depthStencil.depth = depth;
  v.depthStencil.stencil = stencil;
  clear_values_.emplace_back(v);
  info_.clearValueCount = clear_values_.size();
  info_.pClearValues = clear_values_.data();
}

const VkRenderPassBeginInfo *RenderPassBeginInfo::info() const {
  return &info_;
}

CommandBuffer::CommandBuffer(VkCommandBuffer vk_command_buffer_)
    : vk_command_buffer_(vk_command_buffer_) {}

VkCommandBuffer CommandBuffer::handle() const { return vk_command_buffer_; }

bool CommandBuffer::begin(VkCommandBufferUsageFlags flags) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  VkCommandBufferBeginInfo info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                   nullptr, flags, nullptr};
  R_CHECK_VULKAN(vkBeginCommandBuffer(vk_command_buffer_, &info), false)
  return true;
}

bool CommandBuffer::end() const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  R_CHECK_VULKAN(vkEndCommandBuffer(vk_command_buffer_), false)
  return true;
}

bool CommandBuffer::reset(VkCommandBufferResetFlags flags) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  R_CHECK_VULKAN(vkResetCommandBuffer(vk_command_buffer_, flags), false)
  return true;
}

bool CommandBuffer::submit(VkQueue queue, VkFence fence) const {
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &vk_command_buffer_;
  vkQueueSubmit(queue, 1, &submit_info, fence);
  // vkQueueWaitIdle(queue);
  return true;
}

void CommandBuffer::copy(const Buffer &src_buffer, VkDeviceSize src_offset,
                         const Buffer &dst_buffer, VkDeviceSize dst_offset,
                         VkDeviceSize size) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(src_buffer.good(), "using bad buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(dst_buffer.good(), "using bad buffer.")
  const VkBufferCopy copy_region = {src_offset, dst_offset, size};
  vkCmdCopyBuffer(vk_command_buffer_, src_buffer.handle(), dst_buffer.handle(),
                  1, &copy_region);
}

void CommandBuffer::copy(const Image &src_image, VkImageLayout layout,
                         Buffer &dst_buffer,
                         const std::vector<VkBufferImageCopy> &regions) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(src_image.good(), "using bad image.")
  vkCmdCopyImageToBuffer(vk_command_buffer_, src_image.handle(), layout,
                         dst_buffer.handle(), regions.size(), regions.data());
}

void CommandBuffer::copy(const Buffer &src_buffer, Image &dst_image,
                         VkImageLayout layout,
                         const std::vector<VkBufferImageCopy> &regions) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(src_buffer.good(), "using bad buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(dst_image.good(), "using bad image.")
  vkCmdCopyBufferToImage(vk_command_buffer_, src_buffer.handle(),
                         dst_image.handle(), layout, regions.size(),
                         regions.data());
}

void CommandBuffer::copy(const Image &src_image, VkImageLayout src_layout,
                         Image &dst_image, VkImageLayout dst_layout,
                         const std::vector<VkImageCopy> &regions) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(src_image.good(), "using bad image.")
  PONOS_VALIDATE_EXP_WITH_WARNING(dst_image.good(), "using bad image.")
  vkCmdCopyImage(vk_command_buffer_, src_image.handle(), src_layout,
                 dst_image.handle(), dst_layout, regions.size(),
                 regions.data());
}

void CommandBuffer::clear(const Image &image, VkImageLayout layout,
                          const std::vector<VkImageSubresourceRange> &ranges,
                          const VkClearColorValue &color) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(image.good(), "using bad image.")
  vkCmdClearColorImage(vk_command_buffer_, image.handle(), layout, &color,
                       ranges.size(), ranges.data());
}

void CommandBuffer::clear(const Image &image, VkImageLayout layout,
                          const std::vector<VkImageSubresourceRange> &ranges,
                          const VkClearDepthStencilValue &value) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(image.good(), "using bad image.")
  vkCmdClearDepthStencilImage(vk_command_buffer_, image.handle(), layout,
                              &value, ranges.size(), ranges.data());
}

void CommandBuffer::bind(const ComputePipeline &compute_pipeline) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(compute_pipeline.good(), "using bad compute pipeline.")
  vkCmdBindPipeline(vk_command_buffer_, VK_PIPELINE_BIND_POINT_COMPUTE,
                    compute_pipeline.handle());
}

void CommandBuffer::bind(GraphicsPipeline *graphics_pipeline) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(graphics_pipeline->good(), "using bad graphics pipeline.")
  vkCmdBindPipeline(vk_command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline->handle());
}

void CommandBuffer::bind(VkPipelineBindPoint pipeline_bind_point,
                         PipelineLayout *layout, u32 first_set,
                         const std::vector<VkDescriptorSet> &descriptor_sets,
                         const std::vector<u32> &dynamic_offsets) {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(layout && layout->good(), "using bad pipeline layout.")
  vkCmdBindDescriptorSets(
      vk_command_buffer_, pipeline_bind_point, layout->handle(), first_set,
      descriptor_sets.size(), descriptor_sets.data(), dynamic_offsets.size(),
      (dynamic_offsets.size()) ? dynamic_offsets.data() : nullptr);
}

void CommandBuffer::bind(VkPipelineBindPoint pipeline_bind_point,
                         PipelineLayout &pipeline_layout,
                         const std::vector<VkDescriptorSet> &descriptor_sets,
                         const std::vector<u32> &dynamic_offsets,
                         u32 first_set,
                         u32 descriptor_set_count) const {
  if (!descriptor_set_count)
    descriptor_set_count = descriptor_sets.size() - first_set;
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(pipeline_layout.good(), "using bad pipeline layout.")
  vkCmdBindDescriptorSets(vk_command_buffer_, pipeline_bind_point,
                          pipeline_layout.handle(), first_set,
                          descriptor_set_count, descriptor_sets.data(),
                          dynamic_offsets.size(), dynamic_offsets.data());
}

void CommandBuffer::dispatch(u32 x, u32 y, u32 z) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  vkCmdDispatch(vk_command_buffer_, x, y, z);
}

void CommandBuffer::dispatch(const Buffer &buffer, VkDeviceSize offset) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(buffer.good(), "using bad buffer.")
  vkCmdDispatchIndirect(vk_command_buffer_, buffer.handle(), offset);
}

void CommandBuffer::pushConstants(PipelineLayout &pipeline_layout,
                                  VkShaderStageFlags stage_flags,
                                  u32 offset, u32 size,
                                  const void *values) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(pipeline_layout.good(), "using bad pipeline layout.")
  vkCmdPushConstants(vk_command_buffer_, pipeline_layout.handle(), stage_flags,
                     offset, size, values);
}

void CommandBuffer::beginRenderPass(const RenderPassBeginInfo &info,
                                    VkSubpassContents contents) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  vkCmdBeginRenderPass(vk_command_buffer_, info.info(), contents);
}

void CommandBuffer::endRenderPass() const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  vkCmdEndRenderPass(vk_command_buffer_);
}

void CommandBuffer::bindVertexBuffers(
    u32 first_binding, const std::vector<VkBuffer> &buffers,
    const std::vector<VkDeviceSize> &offsets) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  vkCmdBindVertexBuffers(vk_command_buffer_, first_binding, buffers.size(),
                         buffers.data(), offsets.data());
}

void CommandBuffer::bindIndexBuffer(const Buffer &buffer, VkDeviceSize offset,
                                    VkIndexType type) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(buffer.good(), "using bad buffer.")
  vkCmdBindIndexBuffer(vk_command_buffer_, buffer.handle(), offset, type);
}

void CommandBuffer::draw(u32 vertex_count, u32 instance_count,
                         u32 first_vertex, u32 first_instance) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  vkCmdDraw(vk_command_buffer_, vertex_count, instance_count, first_vertex,
            first_instance);
}

void CommandBuffer::drawIndexed(u32 index_count, u32 instance_count,
                                u32 first_index, int32_t vertex_offset,
                                u32 first_instance) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  vkCmdDrawIndexed(vk_command_buffer_, index_count, instance_count, first_index,
                   vertex_offset, first_instance);
}

void CommandBuffer::transitionImageLayout(
    const ImageMemoryBarrier &barrier, VkPipelineStageFlags src_stages,
    VkPipelineStageFlags dst_stages) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  auto barrier_handle = barrier.handle();
  vkCmdPipelineBarrier(vk_command_buffer_, src_stages, dst_stages, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier_handle);
}

void CommandBuffer::blit(const Image &src_image, VkImageLayout src_image_layout,
                         const Image &dst_image, VkImageLayout dst_image_layout,
                         const std::vector<VkImageBlit> &regions,
                         VkFilter filter) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  PONOS_VALIDATE_EXP_WITH_WARNING(src_image.good(), "using bad image.")
  PONOS_VALIDATE_EXP_WITH_WARNING(dst_image.good(), "using bad image.")
  vkCmdBlitImage(vk_command_buffer_, src_image.handle(), src_image_layout,
                 dst_image.handle(), dst_image_layout, regions.size(),
                 &regions[0], filter);
}

void CommandBuffer::setViewport(float width, float height, float min_depth,
                                float max_depth) {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  VkViewport viewport{};

  viewport.width = width;
  viewport.height = height;
  viewport.minDepth = min_depth;
  viewport.maxDepth = max_depth;
  vkCmdSetViewport(vk_command_buffer_, 0, 1, &viewport);
}

void CommandBuffer::setScissor(int32_t offset_x, int32_t offset_y,
                               u32 extent_width, u32 extent_height) {
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_buffer_, "using bad command buffer.")
  VkRect2D scissor_rect{};
  scissor_rect.offset.x = offset_x;
  scissor_rect.offset.y = offset_y;
  scissor_rect.extent.width = extent_width;
  scissor_rect.extent.height = extent_height;
  vkCmdSetScissor(vk_command_buffer_, 0, 1, &scissor_rect);
}

CommandPool::CommandPool(const LogicalDevice::Ref &logical_device,
                         VkCommandPoolCreateFlags parameters,
                         u32 queue_family)
    : logical_device_(logical_device) {
  if (!init(logical_device, parameters, queue_family))
    PONOS_LOG_ERROR("Could not create the command buffer.")
}

CommandPool::~CommandPool() {
  destroy();
}

bool CommandPool::init(const LogicalDevice::Ref &logical_device,
                       VkCommandPoolCreateFlags parameters,
                       u32 queue_family) {
  destroy();
  logical_device_ = logical_device;
  PONOS_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  VkCommandPoolCreateInfo command_pool_create_info = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, // VkStructureType sType
      nullptr,     // const void                 * pNext
      parameters,  // VkCommandPoolCreateFlags     flags
      queue_family // u32                     queueFamilyIndex
  };
  R_CHECK_VULKAN(vkCreateCommandPool(logical_device.handle(),
                                     &command_pool_create_info, nullptr,
                                     &vk_command_pool_), false)
  return true;
}

void CommandPool::destroy() {
  if (logical_device_.good() && vk_command_pool_ != VK_NULL_HANDLE)
    vkDestroyCommandPool(logical_device_.handle(), vk_command_pool_, nullptr);
  vk_command_pool_ = VK_NULL_HANDLE;
}

bool CommandPool::good() const { return vk_command_pool_ != VK_NULL_HANDLE; }

bool CommandPool::allocateCommandBuffers(
    VkCommandBufferLevel level, u32 count,
    std::vector<CommandBuffer> &command_buffers) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_pool_, "using bad command pool.")
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType sType
      nullptr,          // const void             * pNext
      vk_command_pool_, // VkCommandPool            commandPool
      level,            // VkCommandBufferLevel     level
      count             // u32                 commandBufferCount
  };
  std::vector<VkCommandBuffer> vk_command_buffers(count);
  R_CHECK_VULKAN(vkAllocateCommandBuffers(logical_device_.handle(),
                                          &command_buffer_allocate_info,
                                          vk_command_buffers.data()), false)
  command_buffers.clear();
  for (auto cb : vk_command_buffers)
    command_buffers.emplace_back(cb);
  return true;
}

bool CommandPool::freeCommandBuffers(
    std::vector<CommandBuffer> &command_buffers) const {
  if (command_buffers.empty())
    return true;
  PONOS_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_pool_, "using bad command pool.")
  std::vector<VkCommandBuffer> vk_command_buffers(command_buffers.size());
  for (size_t i = 0; i < vk_command_buffers.size(); ++i)
    vk_command_buffers[i] = command_buffers[i].handle();
  vkFreeCommandBuffers(logical_device_.handle(), vk_command_pool_,
                       static_cast<u32>(vk_command_buffers.size()),
                       vk_command_buffers.data());
  command_buffers.clear();
  return true;
}

bool CommandPool::reset(VkCommandPoolResetFlags flags) const {
  PONOS_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  PONOS_VALIDATE_EXP_WITH_WARNING(vk_command_pool_, "using bad command pool.")
  R_CHECK_VULKAN(
      vkResetCommandPool(logical_device_.handle(), vk_command_pool_, flags), false)
  return true;
}

void CommandPool::submitCommandBuffer(
    const LogicalDevice::Ref &logical_device, u32 family_index, VkQueue queue,
    const std::function<void(CommandBuffer &)> &record_callback) {
  circe::vk::CommandPool short_living_command_pool(
      logical_device, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, family_index);
  std::vector<circe::vk::CommandBuffer> short_living_command_buffers;
  short_living_command_pool.allocateCommandBuffers(
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, short_living_command_buffers);
  PONOS_UNUSED_VARIABLE(short_living_command_buffers[0].begin(
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
  record_callback(short_living_command_buffers[0]);
  PONOS_UNUSED_VARIABLE(short_living_command_buffers[0].end());
  Fence submit_fence(logical_device);
  short_living_command_buffers[0].submit(queue, submit_fence.handle());
  submit_fence.wait();
  short_living_command_pool.freeCommandBuffers(short_living_command_buffers);
}

} // namespace circe::vk
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
///\file vk_pipeline.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-01
///
///\brief

#include <circe/vk/pipeline/pipeline.h>
#include <circe/vk/utils/vk_debug.h>
#include <fstream>
#include <utility>

namespace circe::vk {

DescriptorSetLayout::DescriptorSetLayout() = default;

DescriptorSetLayout::DescriptorSetLayout(const LogicalDevice::Ref &logical_device)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) noexcept
    : logical_device_(other.logical_device_),
      vk_descriptor_set_layout_(other.vk_descriptor_set_layout_),
      bindings_(other.bindings_) {
  other.vk_descriptor_set_layout_ = VK_NULL_HANDLE;
}

DescriptorSetLayout::~DescriptorSetLayout() {
  destroy();
}

DescriptorSetLayout &DescriptorSetLayout::operator=(DescriptorSetLayout &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  vk_descriptor_set_layout_ = other.vk_descriptor_set_layout_;
  bindings_ = std::move(other.bindings_);
  other.vk_descriptor_set_layout_ = VK_NULL_HANDLE;
  return *this;
}

bool DescriptorSetLayout::init() {
  destroy();
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  VkDescriptorSetLayoutCreateInfo info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0,
      static_cast<u32>(bindings_.size()),
      (bindings_.size()) ? bindings_.data() : nullptr};
  R_CHECK_VULKAN(vkCreateDescriptorSetLayout(
      logical_device_.handle(), &info, nullptr, &vk_descriptor_set_layout_), false)
  return true;
}

bool DescriptorSetLayout::init(const LogicalDevice::Ref &logical_device) {
  destroy();
  logical_device_ = logical_device;
  return init();
}

void DescriptorSetLayout::destroy() {
  if (logical_device_.good() && vk_descriptor_set_layout_ != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(logical_device_.handle(),
                                 vk_descriptor_set_layout_, nullptr);
  vk_descriptor_set_layout_ = VK_NULL_HANDLE;
}

bool DescriptorSetLayout::good() const { return vk_descriptor_set_layout_ != VK_NULL_HANDLE; }

void DescriptorSetLayout::addLayoutBinding(u32 binding,
                                           VkDescriptorType descriptor_type,
                                           u32 descriptor_count,
                                           VkShaderStageFlags stage_flags) {
  VkDescriptorSetLayoutBinding layout_binding = {
      binding, descriptor_type, descriptor_count, stage_flags, nullptr};
  bindings_.emplace_back(layout_binding);
}

VkDescriptorSetLayout DescriptorSetLayout::handle() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(good(), "Accessing bad DescriptorSetLayout handle.")
  return vk_descriptor_set_layout_;
}

DescriptorSetLayout &PipelineLayout::descriptorSetLayout(u32 id) {
  return descriptor_sets_[id];
}

PipelineLayout::Ref::Ref(const PipelineLayout *pipeline_layout) : pipeline_layout_{pipeline_layout} {}

bool PipelineLayout::Ref::good() const {
  return (pipeline_layout_) != nullptr && pipeline_layout_->good();
}

VkPipelineLayout PipelineLayout::Ref::handle() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(good(), "Accessing bad PipelineLayout handle.")
  return (pipeline_layout_) ? pipeline_layout_->handle() : VK_NULL_HANDLE;
}

PipelineLayout::PipelineLayout() = default;

PipelineLayout::PipelineLayout(const LogicalDevice::Ref &logical_device)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device.good(), "using bad device.")
}

PipelineLayout::PipelineLayout(PipelineLayout &&other) noexcept {
  logical_device_ = other.logical_device_;
  vk_pipeline_layout_ = other.vk_pipeline_layout_;
  other.vk_pipeline_layout_ = VK_NULL_HANDLE;
  descriptor_sets_ = std::move(other.descriptor_sets_);
  vk_push_constant_ranges_ = std::move(other.vk_push_constant_ranges_);
}

PipelineLayout::~PipelineLayout() { destroy(); }

PipelineLayout &PipelineLayout::operator=(PipelineLayout &&other) noexcept {
  logical_device_ = other.logical_device_;
  vk_pipeline_layout_ = other.vk_pipeline_layout_;
  other.vk_pipeline_layout_ = VK_NULL_HANDLE;
  descriptor_sets_ = std::move(other.descriptor_sets_);
  vk_push_constant_ranges_ = std::move(other.vk_push_constant_ranges_);
  return *this;
}

void PipelineLayout::destroy() {
  if (logical_device_.good() && vk_pipeline_layout_ != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(logical_device_.handle(), vk_pipeline_layout_,
                            nullptr);
    vk_pipeline_layout_ = VK_NULL_HANDLE;
  }
}

bool PipelineLayout::init() {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  std::vector<VkDescriptorSetLayout> layout_handles;
  for (auto &ds : descriptor_sets_)
    layout_handles.emplace_back(ds.handle());
  VkPipelineLayoutCreateInfo info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      nullptr,
      0,
      static_cast<u32>(layout_handles.size()),
      layout_handles.data(),
      static_cast<u32>(vk_push_constant_ranges_.size()),
      vk_push_constant_ranges_.data()};
  R_CHECK_VULKAN(vkCreatePipelineLayout(logical_device_.handle(), &info,
                                        nullptr, &vk_pipeline_layout_), false)
  return true;
}

bool PipelineLayout::init(const LogicalDevice::Ref &logical_device) {
  destroy();
  logical_device_ = logical_device;
  return init();
}

VkPipelineLayout PipelineLayout::handle() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(good(), "Accessing bad PipelineLayout handle.")
  return vk_pipeline_layout_;
}

u32 PipelineLayout::createLayoutSet(u32 id) {
  descriptor_sets_.emplace_back(logical_device_);
  return descriptor_sets_.size() - 1;
}

void PipelineLayout::addPushConstantRange(VkShaderStageFlags stage_flags,
                                          u32 offset, u32 size) {
  VkPushConstantRange pc = {stage_flags, offset, size};
  vk_push_constant_ranges_.emplace_back(pc);
}

PipelineLayout::Ref PipelineLayout::ref() const { return PipelineLayout::Ref(this); }

std::vector<DescriptorSetLayout> &PipelineLayout::descriptorSetLayouts() {
  return descriptor_sets_;
}

bool PipelineLayout::good() const {
  return logical_device_.good() && vk_pipeline_layout_ != VK_NULL_HANDLE;
}

DescriptorPool::DescriptorPool() = default;

DescriptorPool::DescriptorPool(const LogicalDevice::Ref &logical_device,
                               u32 max_sets)
    : max_sets_(max_sets), logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
}

DescriptorPool::DescriptorPool(DescriptorPool &&other) noexcept {
  max_sets_ = other.max_sets_;
  logical_device_ = other.logical_device_;
  vk_descriptor_pool_ = other.vk_descriptor_pool_;
  other.vk_descriptor_pool_ = VK_NULL_HANDLE;
  pool_sizes_ = std::move(other.pool_sizes_);
  sets_count_ = other.sets_count_;
}

DescriptorPool::~DescriptorPool() {
  destroy();
}

DescriptorPool &DescriptorPool::operator=(DescriptorPool &&other) noexcept {
  max_sets_ = other.max_sets_;
  logical_device_ = other.logical_device_;
  vk_descriptor_pool_ = other.vk_descriptor_pool_;
  other.vk_descriptor_pool_ = VK_NULL_HANDLE;
  pool_sizes_ = std::move(other.pool_sizes_);
  sets_count_ = other.sets_count_;
  return *this;
}

bool DescriptorPool::init() {
  destroy();
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  VkDescriptorPoolCreateInfo info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr,
      VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, sets_count_,
      static_cast<u32>(pool_sizes_.size()), pool_sizes_.data()};
  R_CHECK_VULKAN(vkCreateDescriptorPool(logical_device_.handle(), &info,
                                        nullptr, &vk_descriptor_pool_), false)
  return true;
}

bool DescriptorPool::init(const LogicalDevice::Ref &logical_device, u32 max_sets) {
  destroy();
  logical_device_ = logical_device;
  max_sets_ = max_sets;
  return init();
}

void DescriptorPool::destroy() {
  if (logical_device_.good() && vk_descriptor_pool_ != VK_NULL_HANDLE)
    vkDestroyDescriptorPool(logical_device_.handle(), vk_descriptor_pool_,
                            nullptr);
}

void DescriptorPool::setPoolSize(VkDescriptorType type,
                                 u32 descriptor_count) {
  sets_count_ += descriptor_count;
  VkDescriptorPoolSize dps = {type, descriptor_count};
  pool_sizes_.emplace_back(dps);
}

VkDescriptorPool DescriptorPool::handle() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(good(), "Accessing bad DescriptorPool handle.")
  return vk_descriptor_pool_;
}

bool DescriptorPool::allocate(
    std::vector<DescriptorSetLayout> &descriptor_set_layouts,
    std::vector<VkDescriptorSet> &descriptor_sets) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  if (vk_descriptor_pool_ == VK_NULL_HANDLE)
    HERMES_LOG_AND_RETURN_VALUE_IF_NOT(init(), false, "unable to init descriptor pool")
  std::vector<VkDescriptorSetLayout> dsls;
  dsls.reserve(descriptor_set_layouts.size());
  for (auto &dsl : descriptor_set_layouts)
    dsls.emplace_back(dsl.handle());
  VkDescriptorSetAllocateInfo allocate_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr,
      vk_descriptor_pool_, static_cast<u32>(dsls.size()), dsls.data()};
//  VkDescriptorSet *descriptor_sets_ptr = nullptr;
  descriptor_sets.clear();
  descriptor_sets.resize(descriptor_set_layouts.size(), VK_NULL_HANDLE);
  R_CHECK_VULKAN(vkAllocateDescriptorSets(
      logical_device_.handle(), &allocate_info, descriptor_sets.data()), false)
  return true;
}

bool DescriptorPool::free(const std::vector<VkDescriptorSet> &descriptor_sets) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_descriptor_pool_, "using bad descriptor pool handle.")
  R_CHECK_VULKAN(
      vkFreeDescriptorSets(logical_device_.handle(), vk_descriptor_pool_,
                           descriptor_sets.size(), descriptor_sets.data()), false)
  return true;
}

bool DescriptorPool::reset() {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(vk_descriptor_pool_, "using bad descriptor pool handle.")
  R_CHECK_VULKAN(
      vkResetDescriptorPool(logical_device_.handle(), vk_descriptor_pool_, 0), false)
  return true;
}

bool DescriptorPool::good() const {
  return logical_device_.good() && vk_descriptor_pool_ != VK_NULL_HANDLE;
}

PipelineShaderStage::PipelineShaderStage() = default;

PipelineShaderStage::PipelineShaderStage(VkShaderStageFlagBits stage,
                                         const ShaderModule &module,
                                         std::string name,
                                         const void *specialization_info_data,
                                         size_t specialization_info_data_size)
    : stage_(stage), module_(module.handle()), name_(std::move(name)) {
  specialization_info_.pData = specialization_info_data;
  specialization_info_.dataSize = specialization_info_data_size;
  specialization_info_.mapEntryCount = 0;
  specialization_info_.pMapEntries = nullptr;
}

PipelineShaderStage::PipelineShaderStage(const PipelineShaderStage &other) {
  specialization_info_ = other.specialization_info_;
  stage_ = other.stage_;
  module_ = other.module_;
  name_ = other.name_;
  map_entries_ = other.map_entries_;
}

PipelineShaderStage::PipelineShaderStage(PipelineShaderStage &&other) noexcept {
  specialization_info_ = other.specialization_info_;
  stage_ = other.stage_;
  module_ = other.module_;
  name_ = other.name_;
  map_entries_ = std::move(other.map_entries_);
}

PipelineShaderStage &PipelineShaderStage::operator=(const PipelineShaderStage &other) {
  specialization_info_ = other.specialization_info_;
  stage_ = other.stage_;
  module_ = other.module_;
  name_ = other.name_;
  map_entries_ = other.map_entries_;
  return *this;
}

PipelineShaderStage &PipelineShaderStage::operator=(PipelineShaderStage &&other) noexcept {
  specialization_info_ = other.specialization_info_;
  stage_ = other.stage_;
  module_ = other.module_;
  name_ = other.name_;
  map_entries_ = std::move(other.map_entries_);
  return *this;
}

void PipelineShaderStage::set(VkShaderStageFlagBits stage,
                              const ShaderModule &module,
                              std::string name,
                              const void *specialization_info_data,
                              size_t specialization_info_data_size) {
  HERMES_VALIDATE_EXP_WITH_WARNING(module.good(), "using bad shader module.")
  stage_ = stage;
  module_ = module.handle();
  name_ = std::move(name);
  specialization_info_.pData = specialization_info_data;
  specialization_info_.dataSize = specialization_info_data_size;
  specialization_info_.mapEntryCount = 0;
  specialization_info_.pMapEntries = nullptr;
}

void PipelineShaderStage::addSpecializationMapEntry(u32 constant_id,
                                                    u32 offset,
                                                    size_t size) {
  VkSpecializationMapEntry map_entry = {constant_id, offset, size};
  map_entries_.emplace_back(map_entry);
  specialization_info_.mapEntryCount = map_entries_.size();
  specialization_info_.pMapEntries = map_entries_.data();
}

VkShaderStageFlagBits PipelineShaderStage::stage() const { return stage_; }

VkShaderModule PipelineShaderStage::module() const { return module_; }

const std::string &PipelineShaderStage::name() const { return name_; }

const VkSpecializationInfo *PipelineShaderStage::specializationInfo() const {
  return &specialization_info_;
}

Pipeline::Pipeline() = default;

Pipeline::Pipeline(const LogicalDevice::Ref &logical_device)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
}

Pipeline::Pipeline(Pipeline &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  vk_pipeline_ = other.vk_pipeline_;
  vk_pipeline_cache_ = other.vk_pipeline_cache_;
  other.vk_pipeline_ = VK_NULL_HANDLE;
  other.vk_pipeline_cache_ = VK_NULL_HANDLE;
  shader_stage_infos_ = std::move(other.shader_stage_infos_);
}

Pipeline::~Pipeline() { destroy(); }

void Pipeline::destroy() {
  if (vk_pipeline_ != VK_NULL_HANDLE) {
    vkDestroyPipeline(logical_device_.handle(), vk_pipeline_, nullptr);
    vk_pipeline_ = VK_NULL_HANDLE;
  }
}

bool Pipeline::saveCache(const std::string &path) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  size_t cache_data_size;
  // Determine the size of the cache data
  R_CHECK_VULKAN(vkGetPipelineCacheData(logical_device_.handle(),
                                        vk_pipeline_cache_, &cache_data_size,
                                        nullptr), false)
  VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
  if (cache_data_size != 0) {
    void *data = new char[cache_data_size];
    if (data) {
      // Retrieve the actual data from the cache
      result = vkGetPipelineCacheData(logical_device_.handle(), vk_pipeline_cache_,
                                      &cache_data_size, data);
      CHECK_VULKAN(result)
      if (result == VK_SUCCESS) {
        std::ofstream ofile(path, std::ios::binary);
        if (ofile.good()) {
          ofile.write((char *) &data, cache_data_size);
          ofile.close();
        }
      }
      delete[] reinterpret_cast<char *>(data);
    }
  }
  return result == VK_SUCCESS;
}

VkPipelineCache Pipeline::cache() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(good() && vk_pipeline_cache_, "Accessing bad Pipeline Cache handle.")
  return vk_pipeline_cache_;
}

VkPipeline Pipeline::handle() const {
  HERMES_VALIDATE_EXP_WITH_WARNING(good(), "Accessing bad Pipeline handle.")
  return vk_pipeline_;
}

void Pipeline::addShaderStage(const PipelineShaderStage &stage) {
  VkPipelineShaderStageCreateInfo info = {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      nullptr,
      0,
      stage.stage(),
      stage.module(),
      stage.name().c_str(),
      stage.specializationInfo()};
  shader_stage_infos_.emplace_back(info);
}

bool Pipeline::good() const {
  return logical_device_.good() && vk_pipeline_ != VK_NULL_HANDLE;
}

ComputePipeline::ComputePipeline() {
  info_.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  info_.pNext = nullptr;
  info_.flags = 0;
}

ComputePipeline::ComputePipeline(const LogicalDevice::Ref &logical_device,
                                 const PipelineShaderStage &stage,
                                 const PipelineLayout::Ref &layout,
                                 ComputePipeline *base_pipeline,
                                 u32 base_pipeline_index)
    : Pipeline(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(layout.good(), "using bad pipeline layout.")
  HERMES_VALIDATE_EXP_WITH_WARNING(base_pipeline ? base_pipeline->good() : true, "using bad base pipeline.")

  info_.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  info_.pNext = nullptr;
  info_.flags = 0;
  info_.layout = layout.handle();
  info_.basePipelineHandle = (base_pipeline ? base_pipeline->handle() : VK_NULL_HANDLE);
  info_.basePipelineIndex = base_pipeline_index;
}

ComputePipeline::ComputePipeline(ComputePipeline &&other) noexcept {
  *this = std::move(other);
}

ComputePipeline &ComputePipeline::operator=(ComputePipeline &&other) noexcept {
  destroy();
  // pipeline
  logical_device_ = other.logical_device_;
  vk_pipeline_ = other.vk_pipeline_;
  vk_pipeline_cache_ = other.vk_pipeline_cache_;
  other.vk_pipeline_ = VK_NULL_HANDLE;
  other.vk_pipeline_cache_ = VK_NULL_HANDLE;
  shader_stage_infos_ = std::move(other.shader_stage_infos_);
  // compute pipeline
  info_ = other.info_;
  return *this;
}

bool ComputePipeline::init(Pipeline *cache) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  // TODO check shader_stage_infos_ array
  info_.stage = this->shader_stage_infos_[0];
  R_CHECK_VULKAN(vkCreateComputePipelines(this->logical_device_.handle(),
                                          (cache ? cache->cache() : VK_NULL_HANDLE), 1, &info_,
                                          nullptr, &this->vk_pipeline_), false)
  return true;
}

GraphicsPipeline::VertexInputState::VertexInputState() {
  info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  info_.pNext = nullptr;
  info_.flags = 0;
  info_.pVertexAttributeDescriptions = nullptr;
  info_.pVertexBindingDescriptions = nullptr;
  info_.vertexAttributeDescriptionCount = 0;
  info_.vertexBindingDescriptionCount = 0;
}

void GraphicsPipeline::VertexInputState::addBindingDescription(
    u32 binding, u32 stride, VkVertexInputRate input_rate) {
  VkVertexInputBindingDescription bd = {binding, stride, input_rate};
  binding_descriptions_.emplace_back(bd);
  info_.vertexBindingDescriptionCount = binding_descriptions_.size();
  info_.pVertexBindingDescriptions = binding_descriptions_.data();
}

void GraphicsPipeline::VertexInputState::addAttributeDescription(
    u32 location, u32 binding, VkFormat format, u32 offset) {
  VkVertexInputAttributeDescription ad = {location, binding, format, offset};
  attribute_descriptions_.emplace_back(ad);
  info_.vertexAttributeDescriptionCount = attribute_descriptions_.size();
  info_.pVertexAttributeDescriptions = attribute_descriptions_.data();
}

const VkPipelineVertexInputStateCreateInfo *
GraphicsPipeline::VertexInputState::info() const {
  return &info_;
}

GraphicsPipeline::ViewportState::ViewportState() {
  info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  info_.pNext = nullptr;
  info_.flags = 0;
  info_.scissorCount = info_.viewportCount = 0;
  info_.pScissors = nullptr;
  info_.pViewports = nullptr;
}

void GraphicsPipeline::ViewportState::addViewport(float x, float y, float width,
                                                  float height, float min_depth,
                                                  float max_depth) {
  VkViewport viewport = {x, y, width, height, min_depth, max_depth};
  viewports_.emplace_back(viewport);
  info_.pViewports = viewports_.data();
  info_.viewportCount = viewports_.size();
}

void GraphicsPipeline::ViewportState::addScissor(int32_t x, int32_t y,
                                                 u32 width,
                                                 u32 height) {
  VkRect2D scissor = {{x, y}, {width, height}};
  scissors_.emplace_back(scissor);
  info_.pScissors = scissors_.data();
  info_.scissorCount = scissors_.size();
}

VkViewport &GraphicsPipeline::ViewportState::viewport(u32 i) {
  HERMES_ASSERT(i < viewports_.size())
  return viewports_[i];
}
VkRect2D &GraphicsPipeline::ViewportState::scissor(u32 i) {
  HERMES_ASSERT(i < scissors_.size())
  return scissors_[i];
}

const VkPipelineViewportStateCreateInfo *
GraphicsPipeline::ViewportState::info() const {
  return &info_;
}

GraphicsPipeline::ColorBlendState::ColorBlendState() {
  info_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  info_.pNext = nullptr;
  info_.flags = 0;
  info_.logicOpEnable = VK_FALSE;
  info_.attachmentCount = 0;
  info_.pAttachments = nullptr;
}

void GraphicsPipeline::ColorBlendState::setLogicOp(VkLogicOp logic_op) {
  info_.logicOpEnable = VK_TRUE;
  info_.logicOp = logic_op;
}

void GraphicsPipeline::ColorBlendState::addAttachmentState(
    VkBool32 blend_enable, VkBlendFactor src_color_blend_factor,
    VkBlendFactor dst_color_blend_factor, VkBlendOp color_blend_op,
    VkBlendFactor src_alpha_blend_factor, VkBlendFactor dst_alpha_blend_factor,
    VkBlendOp alpha_blend_op, VkColorComponentFlags color_write_mask) {
  VkPipelineColorBlendAttachmentState as = {
      blend_enable, src_color_blend_factor, dst_color_blend_factor,
      color_blend_op, src_alpha_blend_factor, dst_alpha_blend_factor,
      alpha_blend_op, color_write_mask};
  attachments_.emplace_back(as);
  info_.pAttachments = attachments_.data();
  info_.attachmentCount = attachments_.size();
}

void GraphicsPipeline::ColorBlendState::setBlendConstants(float r, float g,
                                                          float b, float a) {
  info_.blendConstants[0] = r;
  info_.blendConstants[1] = g;
  info_.blendConstants[2] = b;
  info_.blendConstants[3] = a;
}

const VkPipelineColorBlendStateCreateInfo *
GraphicsPipeline::ColorBlendState::info() const {
  return &info_;
}

GraphicsPipeline::GraphicsPipeline() : Pipeline() {}

GraphicsPipeline::GraphicsPipeline(const LogicalDevice::Ref &logical_device,
                                   const PipelineLayout::Ref &layout,
                                   const RenderPass::Ref &renderpass, u32 subpass,
                                   VkPipelineCreateFlags flags,
                                   GraphicsPipeline *base_pipeline,
                                   u32 base_pipeline_index)
    : Pipeline(logical_device), layout_(layout), renderpass_(renderpass),
      flags_(flags) {
  HERMES_VALIDATE_EXP_WITH_WARNING(base_pipeline ? base_pipeline->good() : true, "using bad base pipeline.")
  info_.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info_.flags = flags;
  info_.subpass = subpass;
  info_.basePipelineHandle =
      base_pipeline ? base_pipeline->handle() : VK_NULL_HANDLE;
  info_.basePipelineIndex = base_pipeline_index;
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept {
  *this = std::move(other);
}

GraphicsPipeline &GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept {
  destroy();
  // pipeline
  logical_device_ = other.logical_device_;
  vk_pipeline_ = other.vk_pipeline_;
  vk_pipeline_cache_ = other.vk_pipeline_cache_;
  other.vk_pipeline_ = VK_NULL_HANDLE;
  other.vk_pipeline_cache_ = VK_NULL_HANDLE;
  shader_stage_infos_ = std::move(other.shader_stage_infos_);
  // graphics pipeline
  vertex_input_state = other.vertex_input_state;
  viewport_state = other.viewport_state;
  color_blend_state = other.color_blend_state;
  layout_ = other.layout_;
  renderpass_ = other.renderpass_;
  flags_ = other.flags_;
  input_assembly_state_ = std::move(other.input_assembly_state_);
  tesselation_state_ = std::move(other.tesselation_state_);
  rasterization_state_ = std::move(other.rasterization_state_);
  multisample_state_ = std::move(other.multisample_state_);
  depth_stencil_state_ = std::move(other.depth_stencil_state_);
  dynamic_state_ = std::move(other.dynamic_state_);
  dynamic_states_ = std::move(other.dynamic_states_);
  info_ = other.info_;
  return *this;
}

bool GraphicsPipeline::init() {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  HERMES_VALIDATE_EXP_WITH_WARNING(layout_.good(), "using bad pipeline layout.")
  HERMES_VALIDATE_EXP_WITH_WARNING(renderpass_.good(), "using bad renderpass.")
  destroy();
  info_.layout = layout_.handle();
  info_.renderPass = renderpass_.handle();
  info_.stageCount = this->shader_stage_infos_.size();
  info_.pStages = this->shader_stage_infos_.data();
  info_.pVertexInputState = vertex_input_state.info();
  info_.pInputAssemblyState = input_assembly_state_.get();
  info_.pTessellationState = tesselation_state_.get();
  info_.pViewportState = viewport_state.info();
  info_.pRasterizationState = rasterization_state_.get();
  info_.pMultisampleState = multisample_state_.get();
  info_.pDepthStencilState = depth_stencil_state_.get();
  info_.pColorBlendState = color_blend_state.info();
  VkPipelineDynamicStateCreateInfo d_info = {
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, // VkStructureType
      nullptr,                                              // pNext
      0, // VkPipelineDynamicStateCreateFlags
      static_cast<u32>(dynamic_states_.size()), // dynamicStateCount;
      (dynamic_states_.size()) ? dynamic_states_.data() : nullptr};
  info_.pDynamicState = &d_info;

  R_CHECK_VULKAN(vkCreateGraphicsPipelines(this->logical_device_.handle(),
                                           VK_NULL_HANDLE, 1, &info_,
                                           nullptr, &this->vk_pipeline_), false)
  return false;
}

void GraphicsPipeline::setLayout(const PipelineLayout::Ref &layout) { layout_ = layout; }

void GraphicsPipeline::setRendePass(const RenderPass::Ref &renderpass) {
  renderpass_ = renderpass;
}

void GraphicsPipeline::setInputState(VkPrimitiveTopology topology,
                                     VkBool32 primitive_restart_enable) {
  input_assembly_state_ =
      std::make_unique<VkPipelineInputAssemblyStateCreateInfo>();
  input_assembly_state_->sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state_->pNext = nullptr;
  input_assembly_state_->flags = 0;
  input_assembly_state_->topology = topology;
  input_assembly_state_->primitiveRestartEnable = primitive_restart_enable;
}

void GraphicsPipeline::setTesselationState(u32 patch_control_points) {
  tesselation_state_ =
      std::make_unique<VkPipelineTessellationStateCreateInfo>();
  tesselation_state_->sType =
      VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  tesselation_state_->pNext = nullptr;
  tesselation_state_->flags = 0;
  tesselation_state_->patchControlPoints = patch_control_points;
}

void GraphicsPipeline::setRasterizationState(
    VkBool32 depth_clamp_enable, VkBool32 rasterizer_discard_enable,
    VkPolygonMode polygon_mode, VkCullModeFlags cull_mode,
    VkFrontFace front_face, VkBool32 depth_bias_enable,
    float depth_bias_constant_factor, float depth_bias_clamp,
    float depth_bias_slope_factor, float line_width) {
  rasterization_state_ =
      std::make_unique<VkPipelineRasterizationStateCreateInfo>();
  rasterization_state_->sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_state_->pNext = nullptr;
  rasterization_state_->flags = 0;
  rasterization_state_->depthClampEnable = depth_clamp_enable;
  rasterization_state_->rasterizerDiscardEnable = rasterizer_discard_enable;
  rasterization_state_->polygonMode = polygon_mode;
  rasterization_state_->cullMode = cull_mode;
  rasterization_state_->frontFace = front_face;
  rasterization_state_->depthBiasEnable = depth_bias_enable;
  rasterization_state_->depthBiasConstantFactor = depth_bias_constant_factor;
  rasterization_state_->depthBiasClamp = depth_bias_clamp;
  rasterization_state_->depthBiasSlopeFactor = depth_bias_slope_factor;
  rasterization_state_->lineWidth = line_width;
}

void GraphicsPipeline::setMultisampleState(
    VkSampleCountFlagBits rasterization_samples, VkBool32 sample_shading_enable,
    float min_sample_shading, const std::vector<VkSampleMask> &sample_mask,
    VkBool32 alpha_to_coverage_enable, VkBool32 alpha_to_one_enable) {
  multisample_state_ = std::make_unique<VkPipelineMultisampleStateCreateInfo>();
  multisample_state_->sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state_->pNext = nullptr;
  multisample_state_->flags = 0;
  multisample_state_->rasterizationSamples = rasterization_samples;
  multisample_state_->sampleShadingEnable = sample_shading_enable;
  multisample_state_->minSampleShading = min_sample_shading;
  multisample_state_->pSampleMask = sample_mask.data();
  multisample_state_->alphaToCoverageEnable = alpha_to_coverage_enable;
  multisample_state_->alphaToOneEnable = alpha_to_one_enable;
}

void GraphicsPipeline::setDepthStencilState(
    VkBool32 depth_test_enable, VkBool32 depth_write_enable,
    VkCompareOp depth_compare_op, VkBool32 depth_bounds_test_enable,
    VkBool32 stencil_test_enable, VkStencilOpState front, VkStencilOpState back,
    float min_depth_bounds, float max_depth_bounds) {
  depth_stencil_state_ =
      std::make_unique<VkPipelineDepthStencilStateCreateInfo>();
  depth_stencil_state_->sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state_->pNext = nullptr;
  depth_stencil_state_->flags = 0;
  depth_stencil_state_->depthTestEnable = depth_test_enable;
  depth_stencil_state_->depthWriteEnable = depth_write_enable;
  depth_stencil_state_->depthCompareOp = depth_compare_op;
  depth_stencil_state_->depthBoundsTestEnable = depth_bounds_test_enable;
  depth_stencil_state_->stencilTestEnable = stencil_test_enable;
  depth_stencil_state_->front = front;
  depth_stencil_state_->back = back;
  depth_stencil_state_->minDepthBounds = min_depth_bounds;
  depth_stencil_state_->maxDepthBounds = max_depth_bounds;
}

[[maybe_unused]] void GraphicsPipeline::addDynamicState(VkDynamicState dynamic_state) {
  if (!dynamic_state_) {
    dynamic_state_ = std::make_unique<VkPipelineDynamicStateCreateInfo>();
    dynamic_state_->sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_->pNext = nullptr;
    dynamic_state_->flags = 0;
  }

  dynamic_states_.emplace_back(dynamic_state);
  dynamic_state_->dynamicStateCount = dynamic_states_.size();
  dynamic_state_->pDynamicStates = dynamic_states_.data();
}

} // namespace circe::vk

//
// Created by filipecn on 09/06/2021.
//


/// Copyright (c) 2021, FilipeCN.
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
///\file hello_vulkan.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-06-09
///
///\brief

#include <circe/vk/utils/base_app.h>
#include <circe/vk/pipeline/pipeline.h>
#include <circe/vk/scene/scene_model.h>
#include <circe/vk/texture/texture.h>
#include <circe/vk/texture/sampler.h>
#include <ponos/geometry/transform.h>

using namespace circe::vk;

struct UniformBufferObject {
  alignas(16) ponos::mat4 model;
  alignas(16) ponos::mat4 view;
  alignas(16) ponos::mat4 proj;
};

class HelloVulkanExample : public BaseApp {
public:
  HelloVulkanExample() : BaseApp({800, 800}, "Hello Vulkan Example") {}

  void recordCommandBuffer(CommandBuffer &cb, u32 i) override {
    Framebuffer &f = framebuffers_[i];
    VkDescriptorSet ds = descriptor_sets[i];
    cb.begin();
    RenderPassBeginInfo renderpass_begin_info(renderpass_.ref(), f);
    renderpass_begin_info.setRenderArea(0, 0, f.size().width, f.size().height);
    renderpass_begin_info.addClearColorValuef(0.f, 0.f, 0.f, 1.f);
    renderpass_begin_info.addClearDepthStencilValue(1, 0);
    cb.beginRenderPass(renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    cb.bind(&pipeline);
    std::vector<VkBuffer> vertex_buffers = {model.vertices().handle()};
    std::vector<VkDeviceSize> offsets = {0};
    cb.bindVertexBuffers(0, vertex_buffers, offsets);
    cb.bindIndexBuffer(model.indices(), 0, VK_INDEX_TYPE_UINT32);
    cb.bind(VK_PIPELINE_BIND_POINT_GRAPHICS,
            &pipeline_layout, 0, {ds});
    cb.drawIndexed(model.indices().size() / sizeof(uint32_t));
    cb.endRenderPass();
    cb.end();
  }

  void prepareFrameImage(uint32_t index) override {
    auto &ubm = uniform_buffer_memories[index];
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
        current_time - start_time)
        .count();
    UniformBufferObject ubo;
    ubo.model =
        ponos::transpose(
            ponos::rotateZ(ponos::DEGREES(time * ponos::RADIANS(90.0f)))
                .matrix());
    ubo.view = ponos::transpose(ponos::Transform::lookAt(ponos::point3(5.0f, 0.0f, 0.0f),
                                                         ponos::point3(0.0f, 0.0f, 0.0f),
                                                         ponos::vec3(0.0f, 1.0f, 0.0f),
                                                         ponos::transform_options::left_handed)
                                    .matrix());
    ubo.proj =
        ponos::transpose(
            ponos::transpose(
                ponos::mat4({1.0f, 0.0f, 0.0f, 0.0f,  //
                             0.0f, -1.0f, 0.0f, 0.0f, //
                             0.0f, 0.0f, 0.5f, 0.0f,  //
                             0.0f, 0.0f, 0.5f, 1.0f})) *
                ponos::Transform::perspective(45.0f, 1.f, 0.1f, 10.0f,
                                              ponos::transform_options::left_handed).matrix());
    ubm.copy(&ubo, sizeof(UniformBufferObject));
  }

  void resize(const ponos::size2 &new_window_size) override {
    auto &vp = pipeline.viewport_state.viewport(0);
    vp.width = new_window_size.width;
    vp.height = new_window_size.height;
    auto s = pipeline.viewport_state.scissor(0);
    s.extent.width = new_window_size.width;
    s.extent.height = new_window_size.height;
  }

  void prepare() override {
    prepareModel();
    preparePipeline();
    prepareUniformBuffers();
    prepareDescriptorSets();
    pipeline_layout.init();
    pipeline.init();
  }

  void prepareModel() {
    auto graphics_queue_family_index_ =
        queue_families_.family("graphics").family_index.value();
    auto graphics_queue_ = queue_families_.family("graphics").vk_queues[0];
    // set vertex layout
    model_vertex_layout.components = {circe::vk::VertexComponent::VERTEX_COMPONENT_POSITION,
                                      circe::vk::VertexComponent::VERTEX_COMPONENT_COLOR,
                                      circe::vk::VertexComponent::VERTEX_COMPONENT_UV};
    model_vertex_layout.fillWithDefaultFormats();
    // init model
    model.setDevice(device_.ref());
    model.setDeviceQueue(graphics_queue_, graphics_queue_family_index_);
    std::string model_path(MODELS_PATH);
    if (!model.loadFromOBJ(model_path + "/chalet.obj", model_vertex_layout))
      return;
    // load texture
    std::string texture_path(TEXTURES_PATH);
    texture = Texture(device_.ref(), texture_path + "/chalet.jpg",
                      graphics_queue_family_index_, graphics_queue_);
    texture_view = texture.image()->view(VK_IMAGE_VIEW_TYPE_2D,
                                         VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_ASPECT_COLOR_BIT);
    texture_sampler = Sampler(device_.ref(), VK_FILTER_LINEAR, VK_FILTER_LINEAR,
                              VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                              VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.f,
                              VK_TRUE, 16, VK_FALSE, VK_COMPARE_OP_ALWAYS, 0.f, 0.f,
                              VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_FALSE);
    // load shaders
    std::string path(VSHADERS_PATH);
    frag_shader_module.setDevice(device_.ref());
    frag_shader_module.load(path + "/frag.spv");
    frag_shader_stage_info.set(VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader_module, "main", nullptr, 0);
    vert_shader_module.setDevice(device_.ref());
    vert_shader_module.load(path + "/vert.spv");
    vert_shader_stage_info.set(VK_SHADER_STAGE_VERTEX_BIT, vert_shader_module, "main", nullptr, 0);
  }

  void preparePipeline() {
    // create pipeline object
    pipeline_layout = PipelineLayout(device_.ref());
    pipeline = GraphicsPipeline(device_.ref(), pipeline_layout.ref(), renderpass_.ref(), 0);
    /////////////////////////////////////// ///////////////////////////////////
    pipeline.vertex_input_state.addBindingDescription(0, model_vertex_layout.stride(), VK_VERTEX_INPUT_RATE_VERTEX);
    for (uint32_t i = 0; i < 3; ++i) {
      auto component = model_vertex_layout[i];
      pipeline.vertex_input_state.addAttributeDescription(i, 0, model_vertex_layout.componentFormat(component),
                                                          model_vertex_layout.componentOffset(component));
    }
    pipeline.addShaderStage(vert_shader_stage_info);
    pipeline.addShaderStage(frag_shader_stage_info);

    pipeline.setInputState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    pipeline.viewport_state.addViewport(0, 0, 800, 800, 0.f, 1.f);
    pipeline.viewport_state.addScissor(0, 0, 800, 800);

    pipeline.setRasterizationState(
        VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
        VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.f, 0.f, 0.f, 1.0f);

    pipeline.setMultisampleState(this->msaa_samples_, VK_FALSE,
                                 1.f, std::vector<VkSampleMask>(), VK_FALSE,
                                 VK_FALSE);

    pipeline.color_blend_state.addAttachmentState(
        VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

    pipeline.setDepthStencilState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS,
                                  VK_FALSE, VK_FALSE, {}, {}, 0.0, 1.0);
    // pipeline.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    // pipeline.addDynamicState(VK_DYNAMIC_STATE_LINE_WIDTH);
  }

  void prepareUniformBuffers() {
    for (size_t i = 0; i < render_engine_.swapchainImageViews().size(); ++i) {
      uniform_buffers.emplace_back(device_.ref(),
                                   sizeof(UniformBufferObject),
                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
      uniform_buffer_memories.emplace_back(uniform_buffers[i], VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      uniform_buffer_memories[i].bind(uniform_buffers[i]);
    }
  }

  void prepareDescriptorSets() {
    int set_count = render_engine_.swapchainImageViews().size();
    descriptor_pool = DescriptorPool(device_.ref(), set_count);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, set_count);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000);
    descriptor_pool.setPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000);
    descriptor_pool.init();
    for (int i = 0; i < set_count; ++i) {
      circe::vk::DescriptorSetLayout &dsl =
          pipeline_layout.descriptorSetLayout(pipeline_layout.createLayoutSet(i));
      dsl.addLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
                           VK_SHADER_STAGE_VERTEX_BIT);
      dsl.addLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                           VK_SHADER_STAGE_FRAGMENT_BIT);
      dsl.init();
    }
    descriptor_pool.allocate(pipeline_layout.descriptorSetLayouts(), descriptor_sets);

    for (int i = 0; i < set_count; ++i) {
      VkDescriptorSet ds = descriptor_sets[i];
      VkBuffer ubo = uniform_buffers[i].handle();
      VkDescriptorBufferInfo buffer_info = {};
      buffer_info.buffer = ubo;
      buffer_info.offset = 0;
      buffer_info.range = sizeof(UniformBufferObject);
      VkDescriptorImageInfo image_info = {};
      image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      image_info.imageView = texture_view.handle();
      image_info.sampler = texture_sampler.handle();

      std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};

      descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptor_writes[0].dstSet = ds;
      descriptor_writes[0].dstBinding = 0;
      descriptor_writes[0].dstArrayElement = 0;
      descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptor_writes[0].descriptorCount = 1;
      descriptor_writes[0].pBufferInfo = &buffer_info;

      descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptor_writes[1].dstSet = ds;
      descriptor_writes[1].dstBinding = 1;
      descriptor_writes[1].dstArrayElement = 0;
      descriptor_writes[1].descriptorType =
          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptor_writes[1].descriptorCount = 1;
      descriptor_writes[1].pImageInfo = &image_info;

      vkUpdateDescriptorSets(device_.handle(),
                             static_cast<uint32_t>(descriptor_writes.size()),
                             descriptor_writes.data(), 0, nullptr);
    }
  }

  // PIPELINE
  PipelineLayout pipeline_layout;
  GraphicsPipeline pipeline;
  // descriptor sets
  DescriptorPool descriptor_pool;
  std::vector<VkDescriptorSet> descriptor_sets;
  // MODEL
  VertexLayout model_vertex_layout;
  ShaderModule frag_shader_module;
  PipelineShaderStage frag_shader_stage_info;
  ShaderModule vert_shader_module;
  PipelineShaderStage vert_shader_stage_info;
  Model model;
  Texture texture;
  Image::View texture_view;
  Sampler texture_sampler;
  // shader resources
  std::vector<Buffer> uniform_buffers;
  std::vector<DeviceMemory> uniform_buffer_memories;
};

int main() {
  return HelloVulkanExample().run();
}


/*
 * Copyright (c) 2018 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <circe/gl/scene/instance_set.h>

#include <utility>

namespace circe::gl {

InstanceSet::View::View(DeviceMemory::View &mem, const VertexAttributes &attributes, GLbitfield access)
    : attributes_(attributes), mem_(mem) {
  mapped_data_ = reinterpret_cast<u8 *>(mem_.mapped(access));
}

InstanceSet::View::~View() {
  if (mapped_data_) {
    mem_.unmap();
    mapped_data_ = nullptr;
  }
}

InstanceSet::View::View(View &&other) noexcept: attributes_{other.attributes_}, mem_{other.mem_} {
  if (mapped_data_) {
    mem_.unmap();
    mapped_data_ = nullptr;
  }
  mapped_data_ = other.mapped_data_;
  other.mapped_data_ = nullptr;
}

InstanceSet::View &InstanceSet::View::operator=(View &&other) noexcept {
  if (mapped_data_) {
    mem_.unmap();
    mapped_data_ = nullptr;
  }
  mapped_data_ = other.mapped_data_;
  other.mapped_data_ = nullptr;
  return *this;
}

std::string InstanceSet::View::memoryDump(hermes::memory_dumper_options options) const {
  auto layout = hermes::MemoryDumper::RegionLayout()
      .withSize(attributes_.stride(), mem_.size() / attributes_.stride());

  layout = layout.withSubRegion(hermes::vec4::memoryDumpLayout().withColor(hermes::ConsoleColors::blue))
      .withSubRegion(hermes::Transform::memoryDumpLayout().withColor(hermes::ConsoleColors::green));
  for (auto attr : attributes_.attributes()) {
  }

  std::string
      dump = hermes::MemoryDumper::dump(mapped_data_, mem_.size(), 16, layout, options);
  return dump;
}

InstanceSet::InstanceSet() = default;

InstanceSet::~InstanceSet() = default;

void InstanceSet::resize(uint n) {
  if (!instance_program.good()) {
    HERMES_LOG_WARNING("Bad instance program!")
    return;
  }
  instance_count_ = n;
  // first we need to split attributes between base and instance attributes
  auto attributes = instance_program.extractAttributes();
  std::set<std::string> base_attribute_names;
  const auto &base_attributes = instance_model.vertexBuffer().attributes.attributes();
  for (const auto &attribute : base_attributes)
    base_attribute_names.insert(attribute.name);

  instance_attributes_ = VertexAttributes();
  for (auto attribute : attributes.attributes()) {
    if (base_attribute_names.find(attribute.name) == base_attribute_names.end()) {
      attribute.divisor = 1;
      instance_attributes_.push(attribute);
    }
  }
  // resize instance buffer
  instance_buffer_.setTarget(GL_ARRAY_BUFFER);
  instance_buffer_.setUsage(GL_STREAM_DRAW);
  instance_buffer_.resize(instance_count_ * instance_attributes_.stride());
  instance_buffer_view_ = std::make_unique<DeviceMemory::View>(instance_buffer_);
  // update vertex attributes
  instance_model.bind();
  instance_attributes_.bindFormats(1);

  instance_model.vertexBuffer().bind();
  CHECK_GL(glBindVertexBuffer(1, instance_buffer_.id(),
                              instance_buffer_view_->offset(), instance_attributes_.stride()));
  instance_model.indexBuffer().bind();

  instance_model.unbind();
  CHECK_GL_ERRORS
}

void InstanceSet::draw(const CameraInterface *camera,
                       hermes::Transform transform) {
  if (!instance_program.good() || !instance_model.vertexBuffer().vertexCount())
    return;
  instance_program.use();
  instance_program.setUniform("model_view_matrix", camera->getViewTransform());
  instance_program.setUniform("projection_matrix", camera->getProjectionTransform());

  instance_model.bind();

  glDrawElementsInstanced(
      instance_model.indexBuffer().element_type,
      instance_model.indexBuffer().element_count * 3,
      instance_model.indexBuffer().data_type,
      nullptr,
      instance_count_);

  instance_model.unbind();

  CHECK_GL_ERRORS;
}

InstanceSet::View InstanceSet::instanceData() {
  return InstanceSet::View(*instance_buffer_view_, instance_attributes_, GL_MAP_WRITE_BIT);
}

} // namespace circe

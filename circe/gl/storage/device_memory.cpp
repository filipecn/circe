/// Copyright (c) 2020, FilipeCN.
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
///\file device_memory.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-20-09
///
///\brief

#include "device_memory.h"

namespace circe::gl {

DeviceMemory::View::View(DeviceMemory &buffer, u64 length, u64 offset)
    : buffer_(buffer), offset_(offset), length_(length) {
  if (!length_)
    length_ = buffer.size();
  if (offset_ >= buffer.size()) {
    spdlog::warn("Offset of View out of bounds. View offset set to 0.");
    offset_ = 0;
  }
  if (offset_ + length_ > buffer.size())
    spdlog::warn("Device Memory View bigger than buffer size. View size reduced.");
  length_ = std::min(offset_ + buffer.size(), buffer.size()) - offset_;
}

DeviceMemory::View &DeviceMemory::View::operator=(DeviceMemory::View &&other) noexcept {
  if (mapped_) /// TODO: is that right? other object might be used the mapped memory?
    buffer_.unmap();
  buffer_ = std::move(other.buffer_);
  offset_ = other.offset_;
  length_ = other.length_;
  mapped_ = other.mapped_;
  return *this;
}

void *DeviceMemory::View::mapped(GLbitfield access) {
  mapped_ = buffer_.mapped(offset_, length_, access);
  return mapped_;
}

void *DeviceMemory::View::mapped(u64 offset_into_view, u64 length, GLbitfield access) {
  mapped_ = buffer_.mapped(offset_ + offset_into_view, length_, access);
  return mapped_;
}

void DeviceMemory::View::unmap() {
  buffer_.unmap();
  mapped_ = nullptr;
}
void DeviceMemory::View::bind() {
  buffer_.bind();
}
std::vector<u8> DeviceMemory::View::rawData() {
  return buffer_.rawData(offset_, length_);
}

DeviceMemory::DeviceMemory() = default;

DeviceMemory::DeviceMemory(GLuint usage, GLuint target, u64 size, void *data) : DeviceMemory() {
  setUsage(usage);
  setTarget(target);
  resize(size);
}

DeviceMemory::~DeviceMemory() {
  destroy();
}

DeviceMemory::DeviceMemory(DeviceMemory &&other) noexcept {
  buffer_object_id_ = other.buffer_object_id_;
  other.buffer_object_id_ = 0;
  target_ = other.target_;
  usage_ = other.usage_;
  size_ = other.size_;
}

DeviceMemory &DeviceMemory::operator=(DeviceMemory &&other) noexcept {
  destroy();
  buffer_object_id_ = other.buffer_object_id_;
  target_ = other.target_;
  usage_ = other.usage_;
  size_ = other.size_;
  other.buffer_object_id_ = 0;
  return *this;
}

void DeviceMemory::setTarget(GLuint _target) {
  target_ = _target;
}

void DeviceMemory::setUsage(GLuint _usage) {
  usage_ = _usage;
}

void DeviceMemory::resize(u64 size_in_bytes) {
  destroy();
  size_ = size_in_bytes;
}

void DeviceMemory::allocate_(void *data) {
  destroy();
  glGenBuffers(1, &buffer_object_id_);
  glBindBuffer(target_, buffer_object_id_);
  CHECK_GL(glBufferData(target_, size_, data, usage_));
}

void DeviceMemory::allocate() {
  allocate_(nullptr);
}

void DeviceMemory::allocate(u64 data_size, void *data) {
  resize(data_size);
  allocate_(data);
}

void DeviceMemory::copy(void *data, u64 data_size, u64 offset) {
  bind();
  CHECK_GL(glBufferSubData(target_, offset, data_size, data));
}

void DeviceMemory::bind() {
  if (!allocated())
    allocate();
  CHECK_GL(glBindBuffer(target_, buffer_object_id_));
}

void *DeviceMemory::mapped(GLenum access) {
  bind();
  void *m = glMapBuffer(target_, access);
  CHECK_GL_ERRORS;
  return m;
}

void *DeviceMemory::mapped(u64 offset, u64 length, GLbitfield access) {
  bind();
  void *m = glMapBufferRange(target_, offset, length, access);
  CHECK_GL_ERRORS;
  return m;
}

void DeviceMemory::unmap() const {
  CHECK_GL(glUnmapBuffer(target_));
}

void DeviceMemory::destroy() {
  if (buffer_object_id_) CHECK_GL(glDeleteBuffers(1, &buffer_object_id_));
  buffer_object_id_ = 0;
}

std::vector<u8> DeviceMemory::rawData(u64 offset, u64 length) {
  if (length == 0)
    length = size_;
  void *m = mapped(offset, length, GL_MAP_READ_BIT);
  std::vector<u8> data(length);
  memcpy(data.data(), m, length);
  unmap();
  return data;
}

}

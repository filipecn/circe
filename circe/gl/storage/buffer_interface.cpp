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
///\file buffer_interface.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-21-09
///
///\brief

#include "buffer_interface.h"

namespace circe::gl {

BufferInterface::BufferInterface() = default;

BufferInterface::~BufferInterface() = default;

void BufferInterface::attachMemory(DeviceMemory &device_memory, u64 offset) {
  mem_ = std::make_unique<DeviceMemory::View>(device_memory, dataSizeInBytes(), offset);
}

void BufferInterface::allocate(GLuint buffer_usage) {
  dm_.setTarget(bufferTarget());
  dm_.setUsage(buffer_usage);
  dm_.resize(dataSizeInBytes());
  mem_ = std::make_unique<DeviceMemory::View>(dm_);
}

void BufferInterface::setData(const void *data) {
  if (!mem_ || !using_external_memory_ ||
      (dataSizeInBytes() != mem_->size() && dm_.allocated())) {
    // allocate if necessary
    allocate(bufferUsage());
  }
  auto *m = mem_->mapped(GL_MAP_WRITE_BIT);
  std::memcpy(m, data, dataSizeInBytes());
  mem_->unmap();
}

void BufferInterface::bind() {
  mem_->bind();
}

}

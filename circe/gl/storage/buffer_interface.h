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
///\file buffer_interface.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-21-09
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_STORAGE_BUFFER_INTERFACE_H
#define PONOS_CIRCE_CIRCE_GL_STORAGE_BUFFER_INTERFACE_H

#include <circe/gl/storage/device_memory.h>

namespace circe::gl {

class BufferInterface {
public:
  BufferInterface();
  virtual ~BufferInterface();
  /// \return
  [[nodiscard]] virtual GLuint bufferTarget() const = 0;
  /// \return
  [[nodiscard]] virtual GLuint bufferUsage() const = 0;
  /// \return
  [[nodiscard]] virtual u64 dataSizeInBytes() const = 0;
  /// Use a pre-existent device buffer to store vertex data
  /// \param device_memory
  /// \param offset
  virtual void attachMemory(DeviceMemory &device_memory, u64 offset);
  /// \param buffer_usage
  virtual void allocate(GLuint buffer_usage);
  /// \param data
  virtual void setData(const void *data);
  /// bind buffer
  virtual void bind();
  /// \return
  inline DeviceMemory::View *memory() { return mem_.get(); }

protected:
  // memory resource
  GLbitfield access_{GL_MAP_WRITE_BIT};
  bool using_external_memory_{false};
  DeviceMemory dm_;
  std::unique_ptr<DeviceMemory::View> mem_;
};

}

#endif //PONOS_CIRCE_CIRCE_GL_STORAGE_BUFFER_INTERFACE_H

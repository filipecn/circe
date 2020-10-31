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
///\file device_memory.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-20-09
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_STORAGE_DEVICE_MEMORY_H
#define PONOS_CIRCE_CIRCE_GL_STORAGE_DEVICE_MEMORY_H

#include <circe/gl/utils/open_gl.h>

namespace circe::gl {

/// Holds an Open GL Buffer Object that store an array of unformatted memory allocated
/// by the GPU. These can be used to store vertex data, pixel data retrieved from
/// images or the framebuffer, and a variety of other things.
///
/// Buffer Targets:
/// GL_ARRAY_BUFFER, GL_ATOMIC_COUNTER_BUFFER, GL_COPY_READ_BUFFER,
/// GL_COPY_WRITE_BUFFER, GL_DRAW_INDIRECT_BUFFER, GL_DISPATCH_INDIRECT_BUFFER,
/// GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER,
/// GL_QUERY_BUFFER, GL_SHADER_STORAGE_BUFFER, GL_TEXTURE_BUFFER,
/// GL_TRANSFORM_FEEDBACK_BUFFER, or GL_UNIFORM_BUFFER.
///
/// Buffer Usages:
/// GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ,
/// GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
///
/// Notes:
/// - This class uses RAII. The object is created on construction and destroyed on
/// deletion.
class DeviceMemory final {
public:
  /// Device memory views allow us to represent and access sub-regions of a device
  /// memory.
  /// Note: Be careful with the destruction order, buffer views must be
  /// destroyed before their respective buffers.
  class View final {
  public:
    friend class DeviceMemory;
    /// \param buffer buffer reference
    /// \param length region size in bytes (0 means entire memory)
    /// \param offset starting position (in bytes) inside device memory
    explicit View(DeviceMemory &buffer, u64 length = 0, u64 offset = 0);
    /// move assign
    View &operator=(View &&other) noexcept;
    [[nodiscard]] inline GLuint bufferId() const { return buffer_.buffer_object_id_; }
    /// \return view size in bytes
    [[nodiscard]] inline u64 size() const { return length_; }
    /// \return view start inside buffer
    [[nodiscard]] inline u64 offset() const { return offset_; }
    /// \return device buffer object
    inline DeviceMemory &deviceMemory() { return buffer_; }
    /// \param access specifies a combination of access flags indicating
    /// the desired access to the range. (GL_MAP_READ_BIT, GL_MAP_WRITE_BIT)
    /// \return pointer to mapped memory
    void *mapped(GLbitfield access);
    void *mapped(u64 offset_into_view, u64 length, GLbitfield access);
    std::vector<u8> rawData();
    /// invalidate mapped pointer and updates the buffer with changed data
    void unmap();
    void bind();

  private:
    DeviceMemory &buffer_;
    u64 offset_{0};
    u64 length_{0};
    void *mapped_{nullptr};
  };
  DeviceMemory();
  /// Parameters constructor
  /// \param usage Specifies the expected usage pattern of the data store.
  /// (ex: GL_STATIC_DRAW)
  /// \param target Specifies the target buffer object. (ex: GL_ARRAY_BUFFER)
  /// \param size Specifies the size in bytes of the buffer object.
  /// \param data Specifies a pointer to data that will be copied into the data
  /// store for initialization.
  DeviceMemory(GLuint usage, GLuint target, u64 size = 0, void *data = nullptr);
  virtual ~DeviceMemory();
  /// Device Memory object cannot be copied.
  DeviceMemory(const DeviceMemory &) = delete;
  DeviceMemory &operator=(const DeviceMemory &) = delete;
  /// Move constructor
  DeviceMemory(DeviceMemory &&other) noexcept;
  /// Self-assignment operator
  DeviceMemory &operator=(DeviceMemory &&other) noexcept;
  /// \param _target Specifies the target buffer object. (ex: GL_ARRAY_BUFFER)
  void setTarget(GLuint _target);
  /// \param usage Specifies the expected usage pattern of the data store.
  /// (ex: GL_STATIC_DRAW)
  void setUsage(GLuint _usage);
  /// \param size Specifies the size in bytes of the buffer object.
  void resize(u64 size_in_bytes);
  /// \return buffer size in bytes
  [[nodiscard]] inline u64 size() const { return size_; }
  /// \return buffer usage
  [[nodiscard]] inline GLuint usage() const { return usage_; }
  /// \return buffer target
  [[nodiscard]] inline GLuint target() const { return target_; }
  /// \return
  [[nodiscard]] inline bool allocated() const { return buffer_object_id_; }
  /// \return
  [[nodiscard]] inline GLuint id() const { return buffer_object_id_; }
  /// Allocates buffer memory on device
  void allocate();
  /// \param data_size in bytes
  /// \param data
  void allocate(u64 data_size, void *data = nullptr);
  /// Copies data to the specified buffer region. Allocates buffer if necessary.
  /// \param data
  /// \param data_size
  /// \param offset
  void copy(void *data, u64 data_size = 0, u64 offset = 0);
  /// Binds buffer object (Allocates first if necessary)
  void bind();
  /// Retrieve a pointer to buffer memory (allocates if necessary).
  /// \param access Specifies the access policy, indicating whether it will be
  /// possible to read from, write to, or both read from and write to the buffer
  /// object's mapped data store. (GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE).
  /// \return
  void *mapped(GLenum access);
  /// Retrieve a pointer to buffer memory region (allocates if necessary).
  /// \param offset start position (in bytes) of mapped memory
  /// \param length mapped memory region size (in bytes)
  /// \param access specifies a combination of access flags indicating
  /// the desired access to the range. (GL_MAP_READ_BIT, GL_MAP_WRITE_BIT)
  /// \return pointer to mapped memory region
  void *mapped(u64 offset, u64 length, GLbitfield access);
  /// invalidate mapped pointer and updates the buffer with changed data
  void unmap() const;
  /// Deletes buffer data and destroy object
  void destroy();
  /// \return
  [[nodiscard]] std::vector<u8> rawData(u64 offset = 0, u64 length = 0);
private:
  void allocate_(void *data);
  GLuint buffer_object_id_{0};
  u64 size_{0};
  GLuint target_{0};            //!< buffer type (GL_ARRAY_BUFFER, ...)
  GLuint usage_{0};             //!< use  (GL_STATIC_DRAW, ...)
};

}

#endif //PONOS_CIRCE_CIRCE_GL_STORAGE_DEVICE_MEMORY_H

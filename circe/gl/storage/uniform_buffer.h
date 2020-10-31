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
///\file uniform_buffer.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-21-09
///
///\brief


#ifndef PONOS_CIRCE_CIRCE_GL_STORAGE_UNIFORM_BUFFER_H
#define PONOS_CIRCE_CIRCE_GL_STORAGE_UNIFORM_BUFFER_H

#include <circe/gl/storage/buffer_interface.h>
#include <circe/gl/graphics/shader.h>

namespace circe::gl {

/// The Uniform Buffer Object (UBO) that is used to store uniform data for a
/// shader program. It can be used to share uniforms between different
/// programs, as well as quickly change between sets of uniforms for the same
/// program object.
class UniformBuffer : public BufferInterface {
public:
  struct UniformBlockData {
    friend class UniformBuffer;
    /// \param buffer
    explicit UniformBlockData(UniformBuffer &buffer) : buffer_(buffer) {}
    /// \return
    inline std::string name() { return name_; }
    template<typename T>
    UniformBlockData &operator=(const T *data) {
      if (sizeof(T) != size_) {
        spdlog::error("Failed to assign data with different size to UBO.\n"
                      "Buffer Size: {0} Data Size: {1}", size_, sizeof(T));
        return *this;
      }
      buffer_.setData(reinterpret_cast<const void *>(data), offset_, size_);
      return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const UniformBlockData &ubd);

  private:
    UniformBuffer &buffer_;
    std::string name_;
    u64 size_{0};
    u64 offset_{0};
    GLuint buffer_binding_{0};
    std::vector<Program::Uniform> variables_;
  };

  UniformBuffer();
  ~UniformBuffer() override;
  [[nodiscard]] GLuint bufferTarget() const override;
  [[nodiscard]] GLuint bufferUsage() const override;
  [[nodiscard]] u64 dataSizeInBytes() const override;
  void setData(const void *data, u64 offset, u64 size);
  void push(const Program &program);
  inline UniformBlockData &operator[](u64 i) {
    return uniform_blocks_[i];
  }
  UniformBlockData &operator[](const std::string &block_name);

  friend std::ostream &operator<<(std::ostream &os, UniformBuffer &uniform_buffer);
private:

  std::vector<UniformBlockData> uniform_blocks_;
  u64 total_size_{0};
  bool needs_update_{false};
};

}

#endif //PONOS_CIRCE_CIRCE_GL_STORAGE_UNIFORM_BUFFER_H

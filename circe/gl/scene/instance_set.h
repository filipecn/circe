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

#ifndef CIRCE_INSTANCE_SET_H
#define CIRCE_INSTANCE_SET_H

#include <circe/gl/scene/scene_object.h>
#include <circe/gl/scene/scene_model.h>

namespace circe::gl {

// *********************************************************************************************************************
//                                                                                                        InstanceSet
// *********************************************************************************************************************
/// Stores a base mesh and its instance descriptions on buffers for fast rendering.
class InstanceSet : public SceneObject {
public:
  class View {
    friend class InstanceSet;
  public:
    View(View &&other) noexcept;
    View(const View &other) = delete;
    ~View();
    View &operator=(const View &other) = delete;
    View &operator=(View &&other) noexcept;
    /// Access a single attribute of an element
    /// Note: The buffer MUST be previously mapped
    /// \tparam T attribute data type
    /// \param attribute_name
    /// \param element_id
    /// \return reference to attribute value
    template<typename T>
    T &at(const std::string &attribute_name, u64 element_id) {
      static T dummy{};
      if (attributes_.attributes().empty()) {
        return dummy;
      }
      if (!attributes_.contains(attribute_name))
        return dummy;
      auto element_address = element_id * attributes_.stride();
      auto attribute_id = attributes_.attributeIndex(attribute_name);
      auto attribute_address = element_address + attributes_.attributeOffset(attribute_id);
      return *reinterpret_cast<T *>(mapped_data_ + attribute_address);
    }
    [[nodiscard]] std::string memoryDump(hermes::memory_dumper_options options =
    hermes::memory_dumper_options::type_values |
        hermes::memory_dumper_options::colored_output) const;
  private:
    explicit View(DeviceMemory::View &mem, const VertexAttributes &attributes, GLbitfield access);
    const VertexAttributes &attributes_;
    DeviceMemory::View &mem_;
    u8 *mapped_data_{nullptr};
  };
  // *******************************************************************************************************************
  //                                                                                                   STATIC METHODS
  // *******************************************************************************************************************
  // *******************************************************************************************************************
  //                                                                                                 FRIEND FUNCTIONS
  // *******************************************************************************************************************
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  InstanceSet();
  ~InstanceSet() override;
  //                                                                                                       assignment
  // *******************************************************************************************************************
  //                                                                                                        OPERATORS
  // *******************************************************************************************************************
  //                                                                                                       assignment
  //                                                                                                       arithmetic
  //                                                                                                          boolean
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  /// \return total number of instances
  inline u64 count() const { return instance_count_; }
  /// reserve memory for n instances
  /// \param n number of instances
  void resize(uint n);
  View instanceData();
  void draw(const CameraInterface *camera, hermes::Transform transform) override;
  // *******************************************************************************************************************
  //                                                                                                    PUBLIC FIELDS
  // *******************************************************************************************************************
  SceneModel instance_model;                       ///< instance model
  Program instance_program;
private:
  DeviceMemory instance_buffer_;                   ///< instance buffer
  std::unique_ptr<DeviceMemory::View> instance_buffer_view_;
  VertexAttributes instance_attributes_;           ///< instance buffer attributes
  size_t instance_count_{0};

  std::vector<GLBufferInterface *> buffers_;      ///< buffers

  std::vector<uint> buffers_indices_;      ///< maps buffers -> data indices
  std::vector<bool> data_changed_;         ///< data of a buffer must be updated
  std::vector<std::vector<uint>> dataU_;   ///< unsigned int data
  std::vector<std::vector<float>> dataF_;  ///< float data
  std::vector<std::vector<uchar>> dataC_;  ///< unsigned byte data
};

} // circe namespace

#endif // CIRCE_INSTANCE_SET_H

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

//
// Created by fuiri on 2/19/2018.
//

#ifndef CIRCE_INSTANCE_SET_H
#define CIRCE_INSTANCE_SET_H

#include <circe/gl/scene/scene_mesh.h>
#include <circe/gl/scene/scene_object.h>

namespace circe::gl {

/// Stores a base mesh and its instances descriptions on buffers for fast
/// rendering.
class InstanceSet : public SceneObject {
public:
  InstanceSet();
  /// \param rm base instance mesh
  /// \param s instance shader
  /// \param n **[optional | def = 0]** number of instances
  explicit InstanceSet(SceneMesh *rm, const ShaderProgram &s, size_t n = 0);
  ~InstanceSet() override;
  /// \param mesh base mesh for all instances
  inline void setInstanceMesh(SceneMesh *mesh) { base_mesh_ = mesh; }
  /// \param shader shader applied to each instance object
  inline void setInstanceShader(const ShaderProgram &shader) { shader_ = shader; }
  /// \return total number of instances
  inline u64 count() const { return count_; }
  /// \param d buffer descriptor
  /// \return index of the new buffer
  uint add(BufferDescriptor d);
  /// reserve memory for n instances
  /// \param n number of instances
  void resize(uint n);
  /// \param b buffer index (must belong to a float type)
  /// \param i instance index
  /// \return pointer to the first component of instance i
  float *instanceF(uint b, uint i);
  /// \param b buffer index (must belong to a uint type)
  /// \param i instance index
  /// \return pointer to the first component of instance i
  uint *instanceU(uint b, uint i);
  /// \param b buffer index
  void bind(uint b);
  void draw(const CameraInterface *camera, ponos::Transform transform) override;

private:
  ShaderProgram shader_;                   ///< instance shader program
  SceneMesh *base_mesh_{nullptr};          ///< base mesh
  uint count_{0};                          ///< number of instances
  std::vector<uint> buffers_indices_;      ///< maps buffers -> data indices
  std::vector<bool> data_changed_;         ///< data of a buffer must be updated
  std::vector<std::vector<uint>> dataU_;   ///< unsigned int data
  std::vector<std::vector<float>> dataF_;  ///< float data
  std::vector<std::vector<uchar>> dataC_;  ///< unsigned byte data
  std::vector<GLBufferInterface *> buffers_; ///< buffers
};

} // circe namespace

#endif // CIRCE_INSTANCE_SET_H

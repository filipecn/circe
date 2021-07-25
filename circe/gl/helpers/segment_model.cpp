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
///\file segment_model.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-02-17
///
///\brief

#include "segment_model.h"
#include <circe/scene/shapes.h>

namespace circe::gl {

SegmentModel::SegmentModel(const hermes::Segment3 &s) {
  // initiate model with unit box
  auto model = Shapes::segment(s);
  mesh_ = std::move(model);
  // shader
  static std::string vs_code = "#version 440 core\nlayout(location = 0) in vec3 position;\n"
                               "uniform mat4 view;\nuniform mat4 projection;\nuniform mat4 model;\n"
                               "void main(){ gl_Position = projection * view * model * vec4(position, 1.0); }";
  static std::string fs_code = "#version 440 core\nlayout(location = 0) out vec4 fragColor;\n"
                               "uniform vec4 color;\nvoid main() { fragColor = color; }";
  std::vector<Shader> shaders;
  shaders.emplace_back(GL_VERTEX_SHADER, vs_code);
  shaders.emplace_back(GL_FRAGMENT_SHADER, fs_code);
  if (!mesh_.program.link(shaders))
    hermes::Log::error("Failed to compile SegmentModel Shader Program:\n {}", mesh_.program.err);
}

SegmentModel::~SegmentModel() = default;

SegmentModel &SegmentModel::operator=(const hermes::Segment3 &s) {
  // TODO
  return *this;
}

void SegmentModel::draw(const CameraInterface *camera, hermes::Transform t) {
  mesh_.program.use();
  mesh_.program.setUniform("color", color);
  mesh_.program.setUniform("view", hermes::transpose(camera->getViewTransform().matrix()));
  mesh_.program.setUniform("projection", hermes::transpose(camera->getProjectionTransform().matrix()));
  mesh_.program.setUniform("model", hermes::transpose(mesh_.transform.matrix()));
  mesh_.draw();
}

}

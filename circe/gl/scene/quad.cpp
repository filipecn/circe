/*
 * Copyright (c) 2017 FilipeCN
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

#include <circe/gl/scene/quad.h>

namespace circe::gl {

Quad::Quad() {
  rawMesh_.meshDescriptor.elementSize = 3;
  rawMesh_.meshDescriptor.count = 2;
  rawMesh_.positionDescriptor.elementSize = 2;
  rawMesh_.positionDescriptor.count = 4;
  rawMesh_.texcoordDescriptor.elementSize = 2;
  rawMesh_.texcoordDescriptor.count = 4;
  rawMesh_.positions = std::vector<float>({0, 0, 1, 0, 1, 1, 0, 1});
  rawMesh_.texcoords = std::vector<float>({0, 0, 1, 0, 1, 1, 0, 1});
  rawMesh_.indices.resize(6);
  rawMesh_.indices[0].positionIndex = rawMesh_.indices[0].texcoordIndex = 0;
  rawMesh_.indices[1].positionIndex = rawMesh_.indices[1].texcoordIndex = 1;
  rawMesh_.indices[2].positionIndex = rawMesh_.indices[2].texcoordIndex = 2;
  rawMesh_.indices[3].positionIndex = rawMesh_.indices[3].texcoordIndex = 0;
  rawMesh_.indices[4].positionIndex = rawMesh_.indices[4].texcoordIndex = 2;
  rawMesh_.indices[5].positionIndex = rawMesh_.indices[5].texcoordIndex = 3;
  rawMesh_.splitIndexData();
  rawMesh_.buildInterleavedData();
  const char *fs = "#version 440 core\n"
                   "in vec2 texCoord;"
                   "out vec4 outColor;"
                   "layout (location = 5) uniform sampler2D tex;"
                   "void main(){"
                   " outColor = texture(tex, texCoord);}";
  const char *vs =
      "#version 440 core\n"
      "layout (location = 0) in vec2 position;"
      "layout (location = 1) in vec2 texcoord;"
      "layout (location = 2) uniform mat4 model;"
      "layout (location = 3) uniform mat4 view;"
      "layout (location = 4) uniform mat4 projection;"
      "out vec2 texCoord;"
      "void main() {"
      " texCoord = texcoord;"
      "gl_Position = projection * view * model * vec4(position, 0.0, 1.0);}";
  shader_.reset(new ShaderProgram(vs, nullptr, fs));
  shader_->addVertexAttribute("position", 0);
  shader_->addVertexAttribute("texcoord", 1);
  shader_->addUniform("model", 2);
  shader_->addUniform("view", 3);
  shader_->addUniform("projection", 4);
  shader_->addUniform("tex", 5);
  this->mesh_ = createSceneMeshPtr(&rawMesh_);
}

void Quad::set(const ponos::point2 &pm, const ponos::point2 &pM) {
  rawMesh_.interleavedData[0] = pm.x;
  rawMesh_.interleavedData[1] = pm.y;
  rawMesh_.interleavedData[4] = pM.x;
  rawMesh_.interleavedData[5] = pm.y;
  rawMesh_.interleavedData[8] = pM.x;
  rawMesh_.interleavedData[9] = pM.y;
  rawMesh_.interleavedData[12] = pm.x;
  rawMesh_.interleavedData[13] = pM.y;
  //   glBindVertexArray(VAO);
  //   this->vb->set(&this->rawMesh->interleavedData[0]);
  //   glBindVertexArray(0);
}

void Quad::draw(const CameraInterface *camera, ponos::Transform t) {
  //   glBindVertexArray(VAO);
  this->mesh_->bind();
  mesh_->vertexBuffer()->locateAttributes(*shader_.get());
  shader_->begin();
  shader_->setUniform("model", ponos::transpose(t.matrix()));
  shader_->setUniform("view",
                      ponos::transpose(camera->getViewTransform().matrix()));
  shader_->setUniform(
      "projection",
      ponos::transpose(camera->getProjectionTransform().matrix()));
  glDrawElements(GL_TRIANGLES,
                 mesh_->indexBuffer()->bufferDescriptor.element_count *
                     mesh_->indexBuffer()->bufferDescriptor.element_size,
                 GL_UNSIGNED_INT, 0);
  CHECK_GL_ERRORS;
  shader_->end();
  //   glBindVertexArray(0);
}

} // namespace circe

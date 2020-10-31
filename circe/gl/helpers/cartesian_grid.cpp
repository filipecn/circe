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

#include <circe/gl/helpers/cartesian_grid.h>

#include <memory>

namespace circe::gl {

CartesianGrid::CartesianGrid() {
  const char *fs = "#version 440 core\n"
                   "out vec4 outColor;"
                   "void main(){"
                   " outColor = vec4(1,0,0,1);}";
  const char *vs = "#version 440 core\n"
                   "layout (location = 0) in vec3 position;"
                   "layout (location = 0) uniform mat4 mvp;"
                   "void main() {"
                   "gl_Position = mvp * vec4(position, 1);}";
  gridShader_ = std::make_shared<ShaderProgram>(vs, nullptr, fs);
  gridShader_->addVertexAttribute("position", 0);
  gridShader_->addUniform("mvp", 0);
}

CartesianGrid::CartesianGrid(int d) : CartesianGrid() {
  xAxisColor = Color::Red();
  yAxisColor = Color::Blue();
  zAxisColor = Color::Green();
  gridColor = Color::Black();
  for (auto &plane : planes) {
    plane.low = -d;
    plane.high = d;
  }
  updateBuffers();
}

CartesianGrid::CartesianGrid(int dx, int dy, int dz) {
  planes[0].low = -dx;
  planes[0].high = dx;
  planes[1].low = -dy;
  planes[1].high = dy;
  planes[2].low = -dz;
  planes[2].high = dz;
  updateBuffers();
}

void CartesianGrid::setDimension(size_t d, int a, int b) {
  planes[d].low = a;
  planes[d].high = b;
  updateBuffers();
}

void CartesianGrid::draw(const CameraInterface *camera, ponos::Transform t) {
  UNUSED_VARIABLE(t);
  glBindVertexArray(VAO_grid_);
  gridShader_->begin();
  gridShader_->setUniform(
      "mvp", ponos::transpose((camera->getProjectionTransform() *
          camera->getViewTransform() *
          camera->getModelTransform() * this->transform)
                                  .matrix()));
  glDrawArrays(GL_LINES, 0, mesh.positions.size());
  CHECK_GL_ERRORS;
  gridShader_->end();
  glBindVertexArray(0);
}

void CartesianGrid::updateBuffers() {
  mesh.clear();
  mesh.meshDescriptor.elementSize = 2;
  mesh.meshDescriptor.count = 0;
  for (int x = planes[0].low; x <= planes[0].high; x++) {
    mesh.addPosition({1.f * x, 1.f * planes[1].low, 0.f});
    mesh.addPosition({1.f * x, 1.f * planes[1].high, 0.f});
    mesh.meshDescriptor.count++;
  }
  for (int y = planes[1].low; y <= planes[1].high; y++) {
    mesh.addPosition({1.f * planes[0].low, 1.f * y, 0.f});
    mesh.addPosition({1.f * planes[0].high, 1.f * y, 0.f});
    mesh.meshDescriptor.count++;
  }
  // YZ
  for (int y = planes[1].low; y <= planes[1].high; y++) {
    mesh.addPosition({0.f, 1.f * y, 1.f * planes[2].low});
    mesh.addPosition({0.f, 1.f * y, 1.f * planes[2].high});
    mesh.meshDescriptor.count++;
  }
  for (int z = planes[2].low; z <= planes[2].high; z++) {
    mesh.addPosition({0.f, 1.f * planes[1].low, 1.f * z});
    mesh.addPosition({0.f, 1.f * planes[1].high, 1.f * z});
    mesh.meshDescriptor.count++;
  }
  // XZ
  for (int x = planes[0].low; x <= planes[0].high; x++) {
    mesh.addPosition({1.f * x, 0.f, 1.f * planes[2].low});
    mesh.addPosition({1.f * x, 0.f, 1.f * planes[2].high});
    mesh.meshDescriptor.count++;
  }
  for (int z = planes[2].low; z <= planes[2].high; z++) {
    mesh.addPosition({1.f * planes[1].low, 0.f, 1.f * z});
    mesh.addPosition({1.f * planes[1].high, 0.f, 1.f * z});
    mesh.meshDescriptor.count++;
  }
  mesh.positionDescriptor.elementSize = 3;
  mesh.positionDescriptor.count = mesh.meshDescriptor.count * 2;
  BufferDescriptor vd = BufferDescriptor::forVertexBuffer(
      3, mesh.positionDescriptor.count, GL_LINES);
  vd.addAttribute(std::string("position"), 3, 0, GL_FLOAT);
  if (VAO_grid_)
    glDeleteBuffers(1, &VAO_grid_);
  glGenVertexArrays(1, &VAO_grid_);
  glBindVertexArray(VAO_grid_);
  vb.reset(new GLVertexBuffer(&mesh.positions[0], vd));
  vb->locateAttributes(*gridShader_.get());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  CHECK_GL_ERRORS;
}

} // namespace circe

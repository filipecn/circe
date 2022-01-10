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
///\file helpers.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-28
///
///\brief

#include <circe/gl/utils/helpers.h>
#include <circe/scene/shapes.h>

namespace circe::gl::helpers {

const char *vs = "#version 440 core\n"
                 "layout(location = 0) in vec3 position;"
                 "uniform mat4 view;"
                 "uniform mat4 projection;"
                 "uniform mat4 model;"
                 "void main() {"
                 "  gl_Position = projection * view * model * vec4(position, 1.0);"
                 "}";

const char *fs = "#version 440 core\n"
                 "layout(location = 0) out vec4 fragColor;"
                 "uniform vec4 color;"
                 "void main() {"
                 "  fragColor = color;"
                 "}";

CameraModel::CameraModel(const CameraInterface *camera) {
  if (camera)
    update(camera);
  setPixelResolution({1, 1});
  target_ = circe::Shapes::box(hermes::bbox3::unitBox(), shape_options::wireframe);
  frustrum_ = circe::Shapes::box({{-1, -1, -1}, {1, 1, 0.9}}, shape_options::wireframe);
  program_.attach(Shader(GL_VERTEX_SHADER, vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, fs));
  HERMES_ASSERT(program_.link())
}

CameraModel::~CameraModel() = default;

void CameraModel::update(const CameraInterface *camera) {
  target_.transform =
      hermes::Transform::translate(hermes::vec3(camera->getTarget())) *
          (hermes::Transform::scale(0.01, 0.01, 0.01) *
              hermes::Transform::translate({-0.5, -0.5, -0.5}));

  frustrum_.transform = hermes::inverse(camera->getProjectionTransform() * camera->getViewTransform());

  pixels_.transform = frustrum_.transform * hermes::Transform::translate({0, 0, -1 + camera->getNear()});
}

void CameraModel::draw(CameraInterface *camera) {
  program_.use();
  program_.setUniform("projection", camera->getProjectionTransform());
  program_.setUniform("view", camera->getViewTransform());
  program_.setUniform("color", color);

  program_.setUniform("model", target_.transform);
  target_.draw();

  program_.setUniform("model", frustrum_.transform);
  frustrum_.draw();

  program_.setUniform("model", pixels_.transform);
  pixels_.draw();

}

void CameraModel::setPixelResolution(const hermes::size2 &resolution) {
  pixels_ = circe::Shapes::plane(hermes::Plane::XY(), {}, {1, 0, 0},
                                 {2, 2}, resolution,
                                 shape_options::wireframe);
}

CartesianGrid::CartesianGrid(size_t resolution) {
  program_.attach(Shader(GL_VERTEX_SHADER, vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, fs));
  HERMES_ASSERT(program_.link())
  resize(hermes::size3(resolution, resolution, resolution));
}

CartesianGrid::~CartesianGrid() = default;

void CartesianGrid::draw(CameraInterface *camera) {
  program_.use();
  program_.setUniform("projection", camera->getProjectionTransform());
  program_.setUniform("view", camera->getViewTransform());
  program_.setUniform("model", hermes::Transform());
  program_.setUniform("color", color);

  xy_.draw();
  xz_.draw();
  yz_.draw();

  base_.draw(camera);
}

void CartesianGrid::resize(const hermes::size3 &size) {
  xy_ = circe::Shapes::plane(hermes::Plane::XY(), {}, {1, 0, 0},
                             hermes::vec2(size.width, size.height),
                             size.slice(0, 1), shape_options::wireframe);
  xz_ = circe::Shapes::plane(hermes::Plane::XZ(), {}, {1, 0, 0},
                             hermes::vec2(size.width, size.depth),
                             size.slice(0, 2), shape_options::wireframe);
  yz_ = circe::Shapes::plane(hermes::Plane::YZ(), {}, {0, 1, 0},
                             hermes::vec2(size.height, size.depth),
                             size.slice(1, 2), shape_options::wireframe);
}

CartesianBase::CartesianBase() {
  program_.attach(Shader(GL_VERTEX_SHADER, vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, fs));
  HERMES_ASSERT(program_.link())
  x_ = circe::Shapes::box({{0, -width, -width},
                           {1, width, width}});
  y_ = circe::Shapes::box({{-width, 0, -width},
                           {width, 1, width}});
  z_ = circe::Shapes::box({{-width, -width, 0},
                           {width, width, 1}});
}

CartesianBase::~CartesianBase() = default;

void CartesianBase::draw(CameraInterface *camera) {
  program_.use();
  program_.setUniform("projection", camera->getProjectionTransform());
  program_.setUniform("view", camera->getViewTransform());
  program_.setUniform("model", hermes::Transform());

  program_.setUniform("color", circe::Color::Red());
  x_.draw();
  program_.setUniform("color", circe::Color::Green());
  y_.draw();
  program_.setUniform("color", circe::Color::Blue());
  z_.draw();
}

}

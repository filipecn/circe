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
///\file helpers.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-28
///
///\brief

#ifndef CIRCE_CIRCE_GL_UTILS_HELPERS_H
#define CIRCE_CIRCE_GL_UTILS_HELPERS_H

#include <circe/scene/camera_interface.h>
#include <circe/gl/scene/scene_model.h>
#include <circe/colors/color.h>
#include <circe/ui/trackball_interface.h>

namespace circe::gl::helpers {

class VectorModel {
public:
  VectorModel();
  ~VectorModel();
  void draw(const hermes::vec3 &v, const CameraInterface *camera);

  circe::Color color{Color::Gray(0.75)};
  f32 width{0.01};
private:
  Program program_;
  SceneModel vector_;
};

class CartesianBase {
public:
  CartesianBase();
  ~CartesianBase();

  void draw(const CameraInterface *camera);

  hermes::Transform transform;
  f32 width{0.01};

private:
  Program program_;
  SceneModel x_;
  SceneModel y_;
  SceneModel z_;
};

class CartesianGrid {
public:
  explicit CartesianGrid(size_t resolution = 10);
  ~CartesianGrid();

  void draw(CameraInterface *camera);

  void resize(const hermes::size3 &size);

  circe::Color color{Color::Gray(0.75)};

private:
  Program program_;
  SceneModel xy_;
  SceneModel xz_;
  SceneModel yz_;
  CartesianBase base_;
};

class CameraModel {
public:
  explicit CameraModel(const CameraInterface *camera = nullptr);
  ~CameraModel();

  void update(const CameraInterface *camera);

  void draw(CameraInterface *camera);

  void setPixelResolution(const hermes::size2 &resolution);

  circe::Color color{Color::Gray(0.75)};

private:
  Program program_;
  SceneModel frustrum_;
  SceneModel target_;
  SceneModel pixels_;
};

class TrackballInterfaceModel {
public:
  explicit TrackballInterfaceModel();
  ~TrackballInterfaceModel();
  void draw(const circe::TrackballInterface &tbi, CameraInterface *camera);

  circe::Color color{Color::Gray(0.75)};
private:
  Program program_;
  SceneModel trackball_;
  SceneModel view_plane_;
};

}

#endif //CIRCE_CIRCE_GL_UTILS_HELPERS_H

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
///\file gizmo.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-01-30
///
///\brief


#ifndef CIRCE_CIRCE_UI_GIZMO_H
#define CIRCE_CIRCE_UI_GIZMO_H

#include <circe/scene/camera_interface.h>
#include <circe/imgui/ImGuizmo.h>

namespace circe {

/// Helper tool to manipulate scene object transformations
class Gizmo {
public:
  static void update(const CameraInterface *camera, hermes::Transform &transform,
                     ImGuizmo::OPERATION operation,
                     ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD) {
    float camera_view[16];
    camera->getViewTransform().matrix().column_major(camera_view);
    float camera_projection[16];
    camera->getProjectionTransform().matrix().column_major(camera_projection);
    float matrix[16];
    transform.matrix().column_major(matrix);
    ImGuizmo::Manipulate(camera_view,
                         camera_projection,
                         operation,
                         mode,
                         matrix,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr);
    transform = hermes::Transform(hermes::mat4(matrix, true));
  }
  ///
  /// \param camera
  /// \param length
  /// \param position
  /// \param size
  /// \param background_color
  static void draw(const CameraInterface *camera,
                   f32 length,
                   hermes::index2 position,
                   hermes::size2 size,
                   ImU32 background_color = 0x10101010) {
    float camera_view[16];
    camera->getViewTransform().matrix().column_major(camera_view);
    ImGuizmo::ViewManipulate(camera_view,
                             length,
                             ImVec2(position.i, position.j),
                             ImVec2(size.width, size.height),
                             background_color);
  }
  ///
  /// \param mode
  /// \param operation
  explicit Gizmo(ImGuizmo::MODE mode = ImGuizmo::MODE::LOCAL,
                 ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE) :
      mode(mode), operation(operation) {}

  void update(const CameraInterface *camera, hermes::Transform &transform) const {
    float camera_view[16];
    camera->getViewTransform().matrix().column_major(camera_view);
    float camera_projection[16];
    camera->getProjectionTransform().matrix().column_major(camera_projection);
    float matrix[16];
    transform.matrix().column_major(matrix);
    ImGuizmo::Manipulate(camera_view,
                         camera_projection,
                         operation,
                         mode,
                         matrix,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr);
    transform = hermes::Transform(hermes::mat4(matrix, true));
  }

  ImGuizmo::MODE mode{ImGuizmo::MODE::LOCAL};
  ImGuizmo::OPERATION operation{ImGuizmo::OPERATION::TRANSLATE};
private:
};

}

#endif //CIRCE_CIRCE_GL_HELPERS_GIZMO_H

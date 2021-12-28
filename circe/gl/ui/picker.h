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
///\file picker.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-13
///
///\brief

#ifndef CIRCE_CIRCE_GL_UI_PICKER_H
#define CIRCE_CIRCE_GL_UI_PICKER_H

#include <circe/gl/io/framebuffer.h>
#include <circe/scene/camera_interface.h>
#include <circe/gl/scene/scene_model.h>

namespace circe::gl {

// *********************************************************************************************************************
//                                                                                                             Picker
// *********************************************************************************************************************
/// This class can be used to pick scene elements from screen positions
class Picker {
public:
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  Picker();
  ~Picker();
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  ///
  /// \param resolution_in_pixels
  virtual void setResolution(const hermes::size2 &resolution_in_pixels);
  ///
  /// \param camera
  /// \param model
  /// \param pick_position
  virtual void pick(const circe::CameraInterface *camera,
                    const hermes::index2 &pick_position, const std::function<void(const Program &)> &f);

  [[nodiscard]] const circe::gl::Texture &result() const;

  u32 picked_index;
protected:
  circe::gl::Program program_;
  circe::gl::Framebuffer fbo_;
  circe::gl::Texture t_object_id_;
};

// *********************************************************************************************************************
//                                                                                                         MeshPicker
// *********************************************************************************************************************
class MeshPicker : public Picker {
public:
  // *******************************************************************************************************************
  //                                                                                                   STATIC METHODS
  // *******************************************************************************************************************
  // *******************************************************************************************************************
  //                                                                                                 FRIEND FUNCTIONS
  // *******************************************************************************************************************
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  MeshPicker();
  ~MeshPicker();
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
  void setResolution(const hermes::size2 &resolution_in_pixels) override;
  void pick(const circe::CameraInterface *camera,
            const hermes::index2 &pick_position, const std::function<void(const Program &)> &f) override;
  // *******************************************************************************************************************
  //                                                                                                    PUBLIC FIELDS
  // *******************************************************************************************************************
  float edge_width{1.f};
  float vertex_width{1.f};
  u32 picked_primitive_index{0};
  u32 picked_edge_index{0};
  u32 picked_vertex_index{0};
  hermes::point3 picked_barycentric_coordinates;

private:
  circe::gl::Texture t_primitive_id_;
  circe::gl::Texture t_edge_id_;
  circe::gl::Texture t_barycentric_;
};

}

#endif //CIRCE_CIRCE_GL_UI_PICKER_H

// Created by filipecn on 7/10/19.
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

#ifndef CIRCE_TEXT_OBJECT_H
#define CIRCE_TEXT_OBJECT_H

#include <circe/gl/scene/scene_object.h>
#include <ponos/structures/raw_mesh.h>

namespace circe::gl {

class TextObject : public SceneObject {
public:
  /// \brief Construct a new Text Object object
  /// \param id **[in]**
  TextObject(int id = -1);
  /// \param text **[in]**
  void setText(const std::string& text);
  void draw(const CameraInterface *c, ponos::Transform t) override;

  float text_size = 1.f;   //!< text scale
  Color text_color;        //!< text color
  ponos::point3f position; //!< text position

private:
  int font_id_ = -1;
  ponos::RawMeshSPtr raw_mesh_;
  ShaderProgramPtr shader_;
  SceneMeshSPtr mesh_;
};

} // namespace circe

#endif // CIRCE_FONT_TEXTURE_H

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

#ifndef CIRCE_SCENE_QUAD_H
#define CIRCE_SCENE_QUAD_H

#include <circe/gl/graphics/shader.h>
#include <circe/gl/scene/scene_object.h>

namespace circe::gl {

/** A quadrilateral scene object. */
class Quad : public SceneMeshObject {
public:
  Quad();
  ~Quad() {}
  /** defines a planar rectangle
   * \param pm minimun extreme
   * \param pM maximum extreme
   */
  void set(const hermes::point2 &pm, const hermes::point2 &pM);
  void draw(const CameraInterface *camera, hermes::Transform transform) override;

private:
  hermes::RawMesh rawMesh_;
};

} // namespace circe
#endif // CIRCE_SCENE_QUAD_H

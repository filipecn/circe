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

#ifndef CIRCE_HELPERS_CARTESIAN_GRID_H
#define CIRCE_HELPERS_CARTESIAN_GRID_H

#include <ponos/ponos.h>

#include <circe/colors/color_palette.h>
#include <circe/gl/scene/scene_object.h>
#include <circe/gl/utils/open_gl.h>

namespace circe::gl {

/* Represents the main planes of a cartesian grid. */
class CartesianGrid : public SceneObject {
public:
  CartesianGrid();
  /* Creates a grid **[-d, d] x [-d, d] x [-d, d]**
   * \param d **[in]** delta in all axis
   */
  explicit CartesianGrid(int d);
  /* Creates a grid **[-dx, dx] x [-dy, dy] x [-dz, dz]**
   * \param dx **[in]** delta X
   * \param dy **[in]** delta Y
   * \param dz **[in]** delta Z
   */
  CartesianGrid(int dx, int dy, int dz);
  /* set
   * \param d **[in]** dimension index (x = 0, ...)
   * \param a **[in]** lowest coordinate
   * \param b **[in]** highest coordinate
   * Set the limits of the grid for an axis.
   *
   * **Example:** If we want a grid with **y** coordinates in **[-5,5]**, we
   *call **set(1, -5, 5)**
   */
  void setDimension(size_t d, int a, int b);
  /* @inherit */
  void draw(const CameraInterface *camera, ponos::Transform t) override;

  Color gridColor;
  Color xAxisColor, yAxisColor, zAxisColor;
  ponos::Interval<int> planes[3];

private:
  void updateBuffers();

  GLuint VAO_grid_ = 0;
  std::shared_ptr<ShaderProgram> gridShader_;
  std::shared_ptr<GLVertexBuffer> vb;
  ponos::RawMesh mesh;
};

} // namespace circe

#endif // CIRCE_HELPERS_CARTESIAN_GRID_H

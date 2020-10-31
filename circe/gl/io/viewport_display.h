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

#ifndef CIRCE_IO_VIEWPORT_DISPLAY_H
#define CIRCE_IO_VIEWPORT_DISPLAY_H

#include <circe/gl/io/display_renderer.h>
#include <circe/ui/ui_camera.h>

#include <ponos/ponos.h>

#include <functional>
#include <memory>

namespace circe::gl {

/** \brief display region
 * Defines a region in the screen.
 */
class ViewportDisplay {
public:
  /* Constructor
   * \param _x **[in]** start pixel in X
   * \param _y **[in]** start pixel in Y
   * \param _width **[in]** width (in pixels)
   * \param _height **[in]** height (in pixels)
   */
  ViewportDisplay(int _x, int _y, int _width, int _height);
  virtual ~ViewportDisplay() = default;
  /* get
   * The point **(x, y)** is mapped from **(0, 0)** and **(x + width, y +
   * height)** to **(1, 1)**
   * \return normalized mouse coordinates relative to the viewport
   */
  ponos::point2 getMouseNPos();
  /// \return true if mouse is inside viewport region
  bool hasMouseFocus() const;
  /* convert
   * \param p **[in]** point (in view space)
   * \return **p** mapped to NDC (**[-1,1]**)
   */
  ponos::point3 viewCoordToNormDevCoord(ponos::point3 p);
  /* convert
   * \param c **[in]** camera
   * \param p **[in]** point (in screen space)
   * \return the unprojected point by the inverse of the camera transform to
   * world space
   */
  ponos::point3 unProject(const CameraInterface &c, ponos::point3 p);
  ponos::point3 unProject();

  void render(const std::function<void(CameraInterface *)> &f = nullptr);
  void mouse(double x, double y);
  void button(int b, int a, int m);
  void scroll(double dx, double dy);
  void key(int k, int scancode, int action, int modifiers);

  // render callback
  std::function<void(const ViewportDisplay&)> prepareRenderCallback;
  std::function<void(CameraInterface *)> renderCallback;
  std::function<void(int, int, int)> buttonCallback;
  std::function<void(double, double)> mouseCallback;
  std::function<void(double, double)> scrollCallback;
  std::function<void(int, int, int, int)> keyCallback;
  std::function<void()> renderEndCallback;

  // viewport
  int x, y, width, height;

  std::shared_ptr<UserCamera> camera;
  std::shared_ptr<DisplayRenderer> renderer;
};

} // namespace circe

#endif // CIRCE_IO_VIEWPORT_DISPLAY_H

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

#include <circe/ui/options.h>
#include <circe/gl/io/display_renderer.h>
#include <circe/ui/ui_camera.h>
#include <circe/colors/color.h>

#include <functional>
#include <memory>

namespace circe::gl {

// *********************************************************************************************************************
//                                                                                                  CameraControlMode
// *********************************************************************************************************************
/// \brief display region
/// Defines a region in the screen.
class ViewportDisplay {
public:
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  /// \param x **[in]** start pixel in X
  /// \param y **[in]** start pixel in Y
  /// \param width **[in]** width (in pixels)
  /// \param height **[in]** height (in pixels)
  ViewportDisplay(int x, int y, int width, int height, circe::viewport_options options = circe::viewport_options::none);
  virtual ~ViewportDisplay() = default;
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  /// \brief This viewport stops processing input
  void disableInput();
  /// \brief Allows this viewport to process input
  void enableInput();
  //                                                                                                             size
  ///
  /// \return
  [[nodiscard]] const hermes::size2 &size() const;
  ///
  /// \return
  [[nodiscard]] const hermes::index2 &position() const;
  ///
  /// \param p
  /// \param s
  void set(const hermes::index2 &p, const hermes::size2 &s);
  //                                                                                                      camera view
  void setCamera(const UICamera &viewport_camera);
  /// \return
  UICamera &camera();
  /// \return
  [[nodiscard]] const UICamera &camera() const;
  /* convert
 * \param p **[in]** point (in view space)
 * \return **p** mapped to NDC (**[-1,1]**)
 */
  [[nodiscard]] hermes::point3 viewCoordToNormDevCoord(hermes::point3 p) const;
  /* convert
   * \param c **[in]** camera
   * \param p **[in]** point (in screen space)
   * \return the unprojected point by the inverse of the camera transform to
   * world space
   */
  hermes::point3 unProject(const CameraInterface &c, hermes::point3 p);
  [[nodiscard]] hermes::point3 unProject(const hermes::point3 &p) const;
  [[nodiscard]] hermes::point3 unProject() const;
  [[nodiscard]] hermes::index2 project(const hermes::point3 &p) const;
  //                                                                                                          queries
  /// \note The point **(x, y)** is mapped from **(0, 0)** and **(x + width, y + height)** to **(1, 1)**
  /// \return normalized mouse coordinates relative to the viewport
  [[nodiscard]] hermes::point2 getMouseNPos() const;
  [[nodiscard]] hermes::index2 getMousePos() const;
  /// \return true if mouse is inside viewport region
  [[nodiscard]] bool hasMouseFocus() const;
  //                                                                                                           update
  void render(const std::function<void(CameraInterface *)> &f = nullptr);
  void mouse(double x, double y);
  void button(int b, int a, int m);
  void scroll(double dx, double dy);
  void key(int k, int scancode, int action, int modifiers) const;
  void resize(int w, int h);
  // *******************************************************************************************************************
  //                                                                                                        CALLBACKS
  // *******************************************************************************************************************
  //                                                                                                 render callbacks
  std::function<void(const ViewportDisplay &)> prepareRenderCallback;
  std::function<void(CameraInterface *)> renderCallback;
  std::function<void()> renderEndCallback;
  //                                                                                                  input callbacks
  std::function<void(int, int, int)> buttonCallback;
  std::function<void(double, double)> mouseCallback;
  std::function<void(double, double)> scrollCallback;
  std::function<void(int, int, int, int)> keyCallback;

  // viewport
  circe::Color clear_screen_color{circe::Color::White()};

  std::shared_ptr<DisplayRenderer> renderer;

private:
  UICamera camera_;
  hermes::size2 resolution_;
  hermes::index2 position_;
  bool input_enabled_{true};
};

} // namespace circe

#endif // CIRCE_IO_VIEWPORT_DISPLAY_H

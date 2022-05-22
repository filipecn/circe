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

#ifndef CIRCE_UI_APP_H
#define CIRCE_UI_APP_H

#include <circe/ui/options.h>
#include <circe/gl/io/graphics_display.h>
#include <circe/gl/io/viewport_display.h>

#include <string>
#include <vector>

namespace circe::gl {

// *********************************************************************************************************************
//                                                                                                                App
// *********************************************************************************************************************
/// \brief Manages viewports_ views and user input
class App {
public:
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  /// \brief Constructor.
  /// \param w **[in]** window width (in pixels)
  /// \param h **[in]** window height (in pixels)
  /// \param t **[in]** window title
  /// \param options
  /// \param viewport_config
  explicit App(uint w,
               uint h,
               const char *t,
               circe::viewport_options viewport_config = circe::viewport_options::none,
               circe::app_options options = circe::app_options::none);
  explicit App(uint w,
               uint h,
               const char *t,
               circe::app_options options,
               circe::viewport_options viewport_config = circe::viewport_options::none);
  ///
  virtual ~App() = default;
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  //                                                                                                        viewports_
  /// \brief Creates a new viewport **[x, y, w, h]** with a perspective camera
  /// \param x **[in]** first pixel in X
  /// \param y **[in]** first pixel in Y
  /// \param w **[in]** viewport width (in pixels)
  /// \param h **[in]** viewport height (in pixels)
  /// \param options
  /// \param viewport_render_callback called on viewport draw
  /// \note the origin of the screen space **(0, 0)** is on the upper-left corner of the window.
  /// \return the id of the new viewport
  size_t addViewport(uint x, uint y, uint w, uint h, circe::viewport_options options = circe::viewport_options::none,
                     const std::function<void(circe::CameraInterface *)> &viewport_render_callback = nullptr);
  /// \brief Creates a new viewport **[x, y, w, h]** with a orthographic camera
  /// \param x **[in]** first pixel in X
  /// \param y **[in]** first pixel in Y
  /// \param w **[in]** viewport width (in pixels)
  /// \param h **[in]** viewport height (in pixels)
  /// \param viewport_render_callback called on viewport draw
  /// \note the origin of the screen space **(0, 0)** is on the upper-left corner of the window.
  /// \return the id of the new viewport
  size_t addViewport2D(uint x, uint y, uint w, uint h,
                       const std::function<void(circe::CameraInterface *)> &viewport_render_callback = nullptr);
  /// \brief Creates a new viewport **[x, y, w, h]** with a orthographic camera
  /// \param x **[in]** first pixel in X
  /// \param y **[in]** first pixel in Y
  /// \param w **[in]** viewport width (in pixels)
  /// \param h **[in]** viewport height (in pixels)
  /// \param viewport_render_callback called on viewport draw
  /// \note the origin of the screen space **(0, 0)** is on the upper-left corner of the window.
  /// \return the id of the new viewport
  size_t addViewport3D(uint x, uint y, uint w, uint h,
                       const std::function<void(circe::CameraInterface *)> &viewport_render_callback = nullptr);
  /// \brief Gets current viewport being rendered
  /// \return current viewport id
  [[nodiscard]] size_t currentRenderingViewport() const;
  /// \brief Gets viewport
  /// \param viewport_id
  /// \return Viewport reference
  [[nodiscard]] const ViewportDisplay &viewport(size_t viewport_id = 0) const;
  /// \brief Gets viewport
  /// \param viewport_id
  /// \return Viewport reference
  ViewportDisplay &viewport(size_t viewport_id = 0);
  //                                                                                                        execution
  /// \brief Initiates graphics display
  void init();
  /// \brief Initiates render loop
  /// \return program result
  int run();
  /// \brief Halts render loop execution
  static void exit();
  //                                                                                                     input update
  virtual void button(int b, int a, int m);
  virtual void mouse(double x, double y);
  virtual void scroll(double dx, double dy);

  // *******************************************************************************************************************
  //                                                                                                        CALLBACKS
  // *******************************************************************************************************************
  //                                                                                                 render callbacks
  std::function<void()> prepareRenderCallback;
  std::function<void(circe::CameraInterface *)> renderCallback;
  std::function<void()> finishRenderCallback;
  //                                                                                                  input callbacks
  std::function<void(unsigned int)> charCallback;
  std::function<void(int, const char **)> dropCallback;
  std::function<void(double, double)> scrollCallback;
  std::function<void(double, double)> mouseCallback;
  std::function<void(int, int, int)> buttonCallback;
  std::function<void(int, int, int, int)> keyCallback;
  std::function<void(int, int)> resizeCallback;

protected:
  bool initialized_;
  size_t window_width_, window_height_;
  std::string title_;
  size_t current_rendering_viewport_{0};
  std::vector<ViewportDisplay> viewports_;

  virtual void render();
  virtual void charFunc(unsigned int pointcode);
  virtual void drop(int count, const char **filenames);
  virtual void key(int key, int scancode, int action, int modifiers);
  virtual void resize(int w, int h);
};

} // namespace circe

#endif // CIRCE_UI_APP_H

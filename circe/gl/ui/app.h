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

#include <circe/gl/io/graphics_display.h>
#include <circe/gl/io/viewport_display.h>

#include <string>
#include <vector>

namespace circe::gl {

/** \brief base class
 * An App makes the creation of viewports easy.
 */
class App {
public:
  /* Constructor.
   * \param w **[in]** window width (in pixels)
   * \param h **[in]** window height (in pixels)
   * \param t **[in]** window title
   * \param defaultViewport **[in | optional]** if true, creates a viewport with
   * the
   * same size of the window
   */
  explicit App(uint w, uint h, const char *t, bool defaultViewport = true);
  virtual ~App() = default;
  /** \brief add
   * \param x **[in]** first pixel in X
   * \param y **[in]** first pixel in Y
   * \param w **[in]** viewport width (in pixels)
   * \param h **[in]** viewport height (in pixels)
   * Creates a new viewport **[x, y, w, h]**.
   *
   * **Note:** the origin of the screen space **(0, 0)** is on the upper-left
   *corner of the window.
   * \return the id of the new viewport
   */
  size_t addViewport(uint x, uint y, uint w, uint h);
  size_t addViewport2D(uint x, uint y, uint w, uint h);
  void init();
  int run();
  void exit();
  template <typename T = UserCamera> T *getCamera(size_t i = 0) {
    return static_cast<T *>(viewports[i].camera.get());
  }

  virtual void button(int b, int a, int m);
  virtual void mouse(double x, double y);
  virtual void scroll(double dx, double dy);

  std::vector<ViewportDisplay> viewports;
  std::function<void()> renderCallback;
  std::function<void(unsigned int)> charCallback;
  std::function<void(int, const char **)> dropCallback;
  std::function<void(double, double)> scrollCallback;
  std::function<void(double, double)> mouseCallback;
  std::function<void(int, int, int)> buttonCallback;
  std::function<void(int, int, int, int)> keyCallback;
  std::function<void(int, int)> resizeCallback;

protected:
  bool initialized;
  size_t windowWidth, windowHeight;
  std::string title;

  virtual void render();
  virtual void charFunc(unsigned int pointcode);
  virtual void drop(int count, const char **filenames);
  virtual void key(int key, int scancode, int action, int modifiers);
  virtual void resize(int w, int h);
};

} // namespace circe

#endif // CIRCE_UI_APP_H

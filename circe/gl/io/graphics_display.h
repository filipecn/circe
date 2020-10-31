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

#ifndef CIRCE_IO_GRAPHICS_DISPLAY_H
#define CIRCE_IO_GRAPHICS_DISPLAY_H

#include <circe/scene/camera_interface.h>
#include <circe/gl/utils/open_gl.h>

#include <ponos/ponos.h>

#include <functional>
#include <memory>

namespace circe::gl {

/** singleton
 *
 * Main class for screen operations. Responsable for getting user input and
 * rendering. The screen origin **(0, 0)** is the lower-left corner of the
 * window.
 */
class GraphicsDisplay {
public:
  ~GraphicsDisplay();
  static GraphicsDisplay &instance() { return instance_; }
  /* set
   * \param w **[in]** width (in pixels)
   * \param h **[in]** height (in pixels)
   * \param windowTitle **[in]**
   * Set window properties.
   */
  void set(int w, int h, const char *windowTitle);
  /* get
   * \param w **[out]** receives width value
   * \param h **[out]** receives height value
   */
  void getWindowSize(int &w, int &h);
  /* get
   * \returns mouse position (screen space)
   */
  ponos::point2 getMousePos();
  /* get
   * \returns mouse position (NDC **[-1, 1]**)
   */
  ponos::point2 getMouseNPos();
  /** \brief convert
   * \param p **[in]** point (in norm dev coordinates)
   * \returns **p** mapped to view coordinates
   */
  ponos::point3 normDevCoordToViewCoord(ponos::point3 p);
  /* convert
   * \param p **[in]** point (in view space)
   * \returns **p** mapped to NDC (**[-1,1]**)
   */
  ponos::point3 viewCoordToNormDevCoord(ponos::point3 p);
  /* convert
   * \param c **[in]** camera
   * \param p **[in]** point (in screen space)
   * \returns the unprojected point by the inverse of the camera transform to
   * world space
   */
  ponos::point3 unProject(const CameraInterface &c, ponos::point3 p);
  /* main loop
   * Starts the application, opens the window and enters in the main loop.
   */
  int start();
  /* exit
   * Closes the window.
   */
  void stop();
  /* query
   * \returns **true** if application is running
   */
  bool isRunning();
  // IO
  void registerCharFunc(const std::function<void(unsigned int)> &f);
  void registerDropFunc(const std::function<void(int, const char **)> &f);
  void registerRenderFunc(const std::function<void()> &f);
  void registerButtonFunc(const std::function<void(int, int, int)> &f);
  void registerKeyFunc(const std::function<void(int, int, int, int)> &f);
  void registerMouseFunc(const std::function<void(double, double)> &f);
  void registerScrollFunc(const std::function<void(double, double)> &f);
  void registerResizeFunc(const std::function<void(int, int)> &f);
  // graphics
  void beginFrame();
  void endFrame();
  /* clear screen
   * \param r **[in]** red
   * \param g **[in]** green
   * \param b **[in]** blue
   * \param a **[in]** alpha
   * Assign the given color **(r, g, b, a)** to all pixels of the screen
   */
  void clearScreen(float r, float g, float b, float a);
  // events
  void processInput();
  // user input
  int keyState(int key);
  GLFWwindow *getGLFWwindow();

  // USER CALLBACKS
  std::function<void()> renderCallback;
  std::function<void(unsigned int)> charCallback;
  std::function<void(int, const char **)> dropCallback;
  std::function<void(int, int, int)> buttonCallback;
  std::function<void(int, int, int, int)> keyCallback;
  std::function<void(double, double)> mouseCallback;
  std::function<void(double, double)> scrollCallback;
  std::function<void(int, int)> resizeCallback;

private:
  static GraphicsDisplay instance_;
  GraphicsDisplay();
  GraphicsDisplay(GraphicsDisplay const &) = delete;
  void operator=(GraphicsDisplay const &) = delete;

  bool init();

  // window
  GLFWwindow *window;
  const char *title;
  int width, height;

  std::vector<std::function<void()>> renderCallbacks;
  std::vector<std::function<void(unsigned int)>> charCallbacks;
  std::vector<std::function<void(int, const char **)>> dropCallbacks;
  std::vector<std::function<void(double, double)>> scrollCallbacks;
  std::vector<std::function<void(double, double)>> mouseCallbacks;
  std::vector<std::function<void(int, int, int)>> buttonCallbacks;
  std::vector<std::function<void(int, int, int, int)>> keyCallbacks;
  std::vector<std::function<void(int, int)>> resizeCallbacks;

  // DEFAULT CALLBACKS
  void charFunc(unsigned int codepoint);
  void dropFunc(int count, const char **filenames);
  void buttonFunc(int button, int action, int modifiers);
  void keyFunc(int key, int scancode, int action, int modifiers);
  void mouseFunc(double x, double y);
  void scrollFunc(double x, double y);
  void resizeFunc(int w, int h);

  // CALLBACKS
  static void error_callback(int error, const char *description);
  static void char_callback(GLFWwindow *window, unsigned int codepoint);
  static void drop_callback(GLFWwindow *window, int count,
                            const char **filenames);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);
  static void button_callback(GLFWwindow *window, int button, int action,
                              int mods);
  static void pos_callback(GLFWwindow *window, double x, double y);
  static void scroll_callback(GLFWwindow *window, double x, double y);
  static void resize_callback(GLFWwindow *window, int w, int h);
};

/* create
 * \param w **[in]** window's width (in pixels)
 * \param h **[in]** window's height (in pixels)
 * \param windowTitle **[in]** window's title
 * \returns reference to GraphicsDiplay with the window created
 */
inline GraphicsDisplay &createGraphicsDisplay(int w, int h,
                                              const char *windowTitle) {
  GraphicsDisplay &gd = GraphicsDisplay::instance();
  gd.set(w, h, windowTitle);
  return gd;
}

} // namespace circe

#endif // CIRCE_IO_GRAPHICS_DISPLAY_H

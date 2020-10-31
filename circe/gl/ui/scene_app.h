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

#ifndef CIRCE_UI_SCENE_APP_H
#define CIRCE_UI_SCENE_APP_H

#include <circe/gl/scene/scene.h>
#include <circe/gl/ui/app.h>

namespace circe::gl {

/** \brief Simple scene with viewports support.
 */
template <template <typename> class StructureType = ponos::Array>
class SceneApp : public App {
public:
  /** \brief Constructor.
   * \param w **[in]** window width (in pixels)
   * \param h **[in]** window height (in pixels)
   * \param t **[in | optional]** window title (empty string by default)
   * \param defaultViewport **[in | optional]** if true, creates a viewport with
   * the same size of the window
   */
  explicit SceneApp(uint w = 800, uint h = 800, const char *t = "",
                    bool defaultViewport = true)
      : App(w, h, t, defaultViewport) {
    selectedObject = nullptr;
    activeObjectViewport = false;
  }
  ~SceneApp() override = default;

  void mouse(double x, double y) override {
    App::mouse(x, y);
    if (selectedObject && selectedObject->active) {
      selectedObject->mouse(*viewports[activeObjectViewport].camera.get(),
                            viewports[activeObjectViewport].getMouseNPos());
      return;
    }
    activeObjectViewport = -1;
    for (size_t i = 0; i < viewports.size(); i++) {
      ponos::point2 p = viewports[i].getMouseNPos();
      if (p >= ponos::point2(-1.f, -1.f) && p <= ponos::point2(1.f, 1.f)) {
        ponos::Ray3 r = viewports[i].camera->pickRay(p);
        if (selectedObject)
          selectedObject->selected = false;
        selectedObject = scene.intersect(r);
        if (selectedObject) {
          selectedObject->selected = true;
          activeObjectViewport = i;
          break;
        }
      }
    }
  }

  void button(int b, int a, int m) override {
    App::button(b, a, m);
    if (selectedObject)
      selectedObject->button(*viewports[activeObjectViewport].camera.get(),
                             viewports[activeObjectViewport].getMouseNPos(), b,
                             a);
  }

  Scene<StructureType> scene; //!< object container

protected:
  void render() override {
    for (size_t i = 0; i < viewports.size(); i++)
      viewports[i].render([&](CameraInterface *c) { scene.render(c); });
    if (this->renderCallback)
      this->renderCallback();
  }

  int activeObjectViewport;
  SceneObject *selectedObject;
};

} // namespace circe

#endif // CIRCE_UI_SCENE_APP_H

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

#ifndef CIRCE_HELPERS_GRID_MODEL_H
#define CIRCE_HELPERS_GRID_MODEL_H

#include <ponos/ponos.h>

#include <circe/colors/color.h>
#include <circe/gl/scene/scene_object.h>
#include <circe/gl/ui/text_renderer.h>
#include <circe/gl/utils/open_gl.h>

#include <functional>
#include <sstream>

namespace circe::gl {

template <typename GridType> class GridModel : public SceneObject {
public:
  GridModel() {}
  GridModel(const GridType *g) : grid(g) {}
  void draw(const CameraInterface *camera, ponos::Transform t) {
    glColor4fv(gridColor.asArray());
    glBegin(GL_LINES); // XY
    for (size_t x = 0; x <= grid->width; x++) {
      glVertex(transform(grid->toWorld(ponos::point2(x, 0))));
      glVertex(transform(grid->toWorld(ponos::point2(x, grid->height))));
    }
    for (size_t y = 0; y <= grid->height; y++) {
      glVertex(transform(grid->toWorld(ponos::point2(0, y))));
      glVertex(transform(grid->toWorld(ponos::point2(grid->width, y))));
    }
    glEnd();
    if (f) {
      glColor4fv(dataColor.asArray());
      grid->forEach(
          [&](const typename GridType::DataType &v, size_t i, size_t j) {
            f((*grid)(i, j), ponos::point3(grid->dataWorldPosition(i, j)));
          });
    }
  }

  std::function<void(const typename GridType::DataType v, ponos::point3 p)> f;

  Color gridColor;
  Color dataColor;

protected:
  const GridType *grid;
};

template <typename T>
class VectorGrid2DModel : public GridModel<ponos::VectorGrid2D<T>> {
public:
  enum class Mode { RAW, EQUAL };
  VectorGrid2DModel() {}
  VectorGrid2DModel(const ponos::VectorGrid2D<T> *g, float sf = 1.f) {
    this->gridColor = Color::Transparent();
    this->dataColor = Color::Red();
    this->grid = g;
    scaleFactor = sf;
    mode = Mode::RAW;
    this->f = [&](const ponos::Vector2<T> v, ponos::point3 p) {
      // glColor4fv(this->dataColor.asArray());
      // glPointSize(3.f);
      // glBegin(GL_POINTS);
      // glVertex(p);
      // glEnd();
      switch (mode) {
      case Mode::RAW:
        draw_vector(ponos::point2(p.x, p.y),
                    scaleFactor * ponos::vec2(v[0], v[1]));
        break;
      case Mode::EQUAL:
        draw_vector(ponos::point2(p.x, p.y),
                    scaleFactor * ponos::normalize(ponos::vec2(v[0], v[1])));
        break;
      }
    };
  }

  float scaleFactor;
  Mode mode;
};

template <typename SGridType> class StaggeredGrid2DModel : public SceneObject {
public:
  StaggeredGrid2DModel(const SGridType *g) {
    this->grid = g;
    u_model.reset(
        new GridModel<typename SGridType::InternalGridType>(&this->grid->u));
    v_model.reset(
        new GridModel<typename SGridType::InternalGridType>(&this->grid->v));
    p_model.reset(
        new GridModel<typename SGridType::InternalGridType>(&this->grid->p));
    setupModel(u_model.get(), Color(0, 0, 0, 0), Color(1, 0, 0, 0.5));
    setupModel(v_model.get(), Color(0, 0, 0, 0), Color(0, 0, 1, 0.5));
  }
  void draw(const CameraInterface *camera, ponos::Transform t) override {
    u_model->draw();
    v_model->draw();
    p_model->draw();
  }
  void setupModel(GridModel<typename SGridType::InternalGridType> *model,
                  Color gridColor, Color dataColor) {
    model->gridColor = gridColor;
    model->dataColor = dataColor;
    model->f = [this, gridColor, dataColor](float v, ponos::point3 p) {
      // glPointSize(4);
      // glBegin(GL_POINTS);
      // glVertex(p);
      // glEnd();
      std::ostringstream stringStream;
      stringStream << v;
      std::string copyOfStr = stringStream.str();
      //      text->render(copyOfStr, glGetMVPTransform()(p), 0.4f, dataColor);
    };
  }

  std::shared_ptr<GridModel<typename SGridType::InternalGridType>> u_model;
  std::shared_ptr<GridModel<typename SGridType::InternalGridType>> v_model;
  std::shared_ptr<GridModel<typename SGridType::InternalGridType>> p_model;

private:
  //  const SGridType *grid;
  TextRenderer *text;
};

} // namespace circe

#endif // CIRCE_HELPERS_GRID_MODEL_H

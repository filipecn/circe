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

#ifndef CIRCE_HELPERS_QUAD_TREE_MODEL_H
#define CIRCE_HELPERS_QUAD_TREE_MODEL_H

#include <ponos/ponos.h>

#include <circe/colors/color_palette.h>
#include <circe/gl/helpers/geometry_drawers.h>
#include <circe/gl/scene/scene_object.h>
#include <circe/gl/ui/text_renderer.h>
#include <circe/gl/utils/open_gl.h>

#include <functional>
#include <sstream>

namespace circe::gl {

template <typename QT> class QuadTreeModel : public SceneObject {
public:
  QuadTreeModel() : tree(nullptr) {}
  QuadTreeModel(const QT *qt) : tree(qt) { edgesColor = Color::Black(); }
  void draw() const {
    tree->traverse([this](const typename QT::Node &node) -> bool {
      glColor4fv(edgesColor.asArray());
      draw_bbox(node.region());
      if (drawCallback)
        drawCallback(node);
      return true;
    });
  }
  std::function<void(const typename QT::Node &n)> drawCallback;
  Color edgesColor;
  ColorPalette colorPallete;

private:
  const QT *tree;
};

} // namespace circe

#endif // CIRCE_HELPERS_QUAD_TREE_MODEL_H

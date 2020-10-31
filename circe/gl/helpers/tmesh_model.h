// Created by filipecn on 3/15/18.
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

#ifndef CIRCE_TMESH_MODEL_H
#define CIRCE_TMESH_MODEL_H

#include <circe/colors/color_palette.h>
#include <circe/scene/scene_object.h>
#include <ponos/ponos.h>

namespace circe {

template <typename V = float, typename F = float, typename E = float,
          typename P = float>
class TMeshModel : public circe::SceneObject {
public:
  explicit TMeshModel(const ponos::TMesh<V, F, E, P> &m) : mesh_(m) {
    selectedEdge_ = selectedFace_ = selectedVertex_ = -1;
    edgesColor = Color(0, 0, 0, 0.2);
    selectedEdgeColor = COLOR_BLACK;
    selectedFaceColor = Color(0.8, 0.1, 0.4, 0.2);
    facesColor = Color(0.8, 0.1, 0.4, 0.2);
    selectedFaceColor.a = 1.0;
    for (size_t i = 0; i < mesh_.tetrahedron.size(); i++) {
      auto v = mesh_.tetrahedronVertices(i);
      centroids_.emplace_back((mesh_.vertices[v[0]].position +
                               ponos::vec3(mesh_.vertices[v[1]].position) +
                               ponos::vec3(mesh_.vertices[v[2]].position) +
                               ponos::vec3(mesh_.vertices[v[3]].position)) /
                              4.f);
    }
  }

  void draw(const CameraInterface *camera, ponos::Transform t) override {
    /*GL_DRAW_POINTS(4.f, glColor(COLOR_BLACK);
                   for (size_t i = 0; i < mesh_.vertices.size(); i++)
                       glVertex(mesh_.vertices[i].position);

    )
    glColor(edgesColor);
    GL_DRAW_LINES(2.f,
                  for (auto &e
                       : mesh_.edges) {
                    glVertex(mesh_.vertices[e.a].position);
                    glVertex(mesh_.vertices[e.b].position);
                  }

    )
    glColor(facesColor);
    GL_DRAW_TRIANGLES(for (size_t f = 0; f < mesh_.faces.size(); f++) {
      auto v = mesh_.faceVertices(f);
      for (size_t k = 0; k < 3; k++)
        glVertex(mesh_.vertices[v[k]].position);
    })*/
  }

  bool intersect(const ponos::Ray3 &r, float *t) override {
    selectedVertex_ = selectedFace_ = selectedEdge_ = -1;
    UNUSED_VARIABLE(r);
    UNUSED_VARIABLE(t);
    return false;
  }
  Color selectedEdgeColor;
  Color edgesColor;
  Color selectedFaceColor;
  Color facesColor;

  std::function<void(const typename ponos::TMesh<V, F, E, P>::Vertex &)>
      vertexCallback;

private:
  std::vector<ponos::point3> centroids_;
  int selectedVertex_;
  int selectedFace_;
  int selectedEdge_;
  const ponos::TMesh<V, F, E, P> &mesh_;
};

} // namespace circe

#endif // CIRCE_TMESH_MODEL_H

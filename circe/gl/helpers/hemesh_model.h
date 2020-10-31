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

#ifndef CIRCE_HELPERS_HEMESH_MODEL_H
#define CIRCE_HELPERS_HEMESH_MODEL_H

#include <circe/scene/scene_object.h>
#include <circe/ui/text_renderer.h>
#include <circe/utils/open_gl.h>

namespace circe {

class HEMeshObject : public circe::SceneObject {
public:
  HEMeshObject(const ponos::HEMesh2DF *m, TextRenderer *t, float de = 0.03f,
               float dv = 0.015f) {
    // mesh.reset(new ponos::HEMesh2DF(rm));
    mesh = m;
    // std::vector<size_t> p(1, 0);
    // ponos::fastMarch2D(mesh.get(), p);
    text = t;
    distanceFromEdge = de;
    distanceFromVertex = dv;
  }

  void draw(const CameraInterface *camera,
            ponos::Transform transform) override {
    const std::vector<ponos::HEMesh2DF::Vertex> &vertices = mesh->getVertices();
    const std::vector<ponos::HEMesh2DF::Edge> &edges = mesh->getEdges();
    const std::vector<ponos::HEMesh2DF::Face> &faces = mesh->getFaces();
    glPointSize(4.f);
    glColor4f(0, 0, 0, 0.5);
    glBegin(GL_POINTS);
    for (auto p : vertices)
      circe::glVertex(p.position);
    glEnd();
    for (unsigned long int k = 0; k < vertices.size(); k++) {
      char label[100];
      sprintf(label, "%lu", k);
      ponos::point3 labelPosition = glGetMVPTransform()(
          ponos::point3(vertices[k].position[0], vertices[k].position[1], 0));
      //      text->render(label, labelPosition, .5f,
      //                   circe::Color(0.8f, 0.2f, 0.7f, 0.1f));
      //      sprintf(label, " %f", vertices[k].data);
      //      text->render(label, labelPosition, .3f, circe::Color(1.0f, 0.2f,
      //      0.4f));
    }
    int k = 0;
    for (auto e : edges) {
      // std::cout << "edge " << k << " = " << e.orig
      //	<< " | " << e.dest << std::endl;
      glColor4f(0, 0, 0, 0.5);
      ponos::point2 a = vertices[e.orig].position;
      ponos::point2 b = vertices[e.dest].position;
      ponos::vec2 v = ponos::normalize(b - a);
      glBegin(GL_LINES);
      circe::glVertex(a);
      circe::glVertex(b);
      glEnd();
      glColor4f(1, 0, 0, 0.3);
      ponos::point2 A =
          a + distanceFromEdge * v.left() + distanceFromVertex * v;
      ponos::point2 B =
          b + distanceFromEdge * v.left() - distanceFromVertex * v;
      ponos::point2 C = A + (B - A) / 2.f;
      ponos::point3 labelPositition =
          glGetMVPTransform()(ponos::point3(C.x, C.y, 0.f));
      char label[100];
      sprintf(label, "%d", k++);
      //      text->render(label, labelPositition, .3f,
      //                   circe::Color(0.4f, 0.2f, 0.7f, 0.3f));
      circe::draw_vector(A, B - A, 0.04, 0.05);
      glColor4f(1, 0, 0, 0.2);
      glBegin(GL_LINES);
      if (e.next >= 0) {
        circe::glVertex(B);
        ponos::vec2 V = ponos::normalize(vertices[edges[e.next].dest].position -
                                         vertices[edges[e.next].orig].position);
        circe::glVertex(vertices[edges[e.next].orig].position +
                        distanceFromEdge * V.left() + distanceFromVertex * V);
      }
      if (e.prev >= 0) {
        circe::glVertex(A);
        ponos::vec2 V = ponos::normalize(vertices[edges[e.prev].dest].position -
                                         vertices[edges[e.prev].orig].position);
        circe::glVertex(vertices[edges[e.prev].dest].position +
                        distanceFromEdge * V.left() - distanceFromVertex * V);
      }
      glEnd();
    }
    for (unsigned long int i = 0; i < faces.size(); i++) {
      ponos::point2 mp(0, 0);
      glColor4f(0, 1, 0, 0);
      glBegin(GL_TRIANGLES);
      mesh->traversePolygonEdges(i, [&mp, &edges, &vertices](int e) {
        circe::glVertex(vertices[edges[e].orig].position);
        mp[0] += vertices[edges[e].orig].position[0];
        mp[1] += vertices[edges[e].orig].position[1];
      });
      glEnd();
      char label[100];
      ponos::point3 labelPosition =
          glGetMVPTransform()(ponos::point3(mp[0] / 3.f, mp[1] / 3.f, 0.f));
      sprintf(label, "%lu", i);
      //      text->render(label, labelPosition, .5f,
      //                   circe::Color(0.8f, 0.5f, 0.2f, 0.2f));
    }
  }

  TextRenderer *text;
  float distanceFromEdge;
  float distanceFromVertex;

private:
  const ponos::HEMesh2DF *mesh;
};

} // namespace circe

#endif // CIRCE_HELPERS_HEMESH_MODEL_H

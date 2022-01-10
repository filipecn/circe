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

#include <circe/gl/helpers/geometry_drawers.h>
#include <hermes/numeric/numeric.h>

namespace circe::gl {

void fill_box(const hermes::point2 &a, const hermes::point2 &b) {
  HERMES_UNUSED_VARIABLE(a);
  HERMES_UNUSED_VARIABLE(b);
  //  glBegin(GL_QUADS);
  //  glVertex(hermes::point2(a.x, a.y));
  //  glVertex(hermes::point2(b.x, a.y));
  //  glVertex(hermes::point2(b.x, b.y));
  //  glVertex(hermes::point2(a.x, b.y));
  //  glEnd();
}

void draw_bbox(const hermes::bbox2 &bbox, float *fillColor) {
  HERMES_UNUSED_VARIABLE(bbox);
  HERMES_UNUSED_VARIABLE(fillColor);
  //  glBegin(GL_LINE_LOOP);
  //  glVertex(hermes::point2(bbox.lower.x, bbox.lower.y));
  //  glVertex(hermes::point2(bbox.upper.x, bbox.lower.y));
  //  glVertex(hermes::point2(bbox.upper.x, bbox.upper.y));
  //  glVertex(hermes::point2(bbox.lower.x, bbox.upper.y));
  //  glEnd();
  //  if (fillColor) {
  //    glColor4fv(fillColor);
  //    glBegin(GL_QUADS);
  //    glVertex(hermes::point2(bbox.lower.x, bbox.lower.y));
  //    glVertex(hermes::point2(bbox.upper.x, bbox.lower.y));
  //    glVertex(hermes::point2(bbox.upper.x, bbox.upper.y));
  //    glVertex(hermes::point2(bbox.lower.x, bbox.upper.y));
  //    glEnd();
  //  }
}

void draw_bbox(const hermes::bbox2 &bbox, const Color &edgeColor,
               const Color &fillColor) {
  HERMES_UNUSED_VARIABLE(bbox);
  HERMES_UNUSED_VARIABLE(edgeColor);
  HERMES_UNUSED_VARIABLE(fillColor);
  //  glColor4fv(edgeColor.asArray());
  //  glBegin(GL_LINE_LOOP);
  //  glVertex(hermes::point2(bbox.lower.x, bbox.lower.y));
  //  glVertex(hermes::point2(bbox.upper.x, bbox.lower.y));
  //  glVertex(hermes::point2(bbox.upper.x, bbox.upper.y));
  //  glVertex(hermes::point2(bbox.lower.x, bbox.upper.y));
  //  glEnd();
  //  glColor4fv(fillColor.asArray());
  //  glBegin(GL_QUADS);
  //  glVertex(hermes::point2(bbox.lower.x, bbox.lower.y));
  //  glVertex(hermes::point2(bbox.upper.x, bbox.lower.y));
  //  glVertex(hermes::point2(bbox.upper.x, bbox.upper.y));
  //  glVertex(hermes::point2(bbox.lower.x, bbox.upper.y));
  //  glEnd();
}

void draw_bbox(const hermes::bbox3 &bbox) {
  HERMES_UNUSED_VARIABLE(bbox);
  //  glBegin(GL_LINE_LOOP);
  //  glVertex(hermes::point3(bbox.lower.x, bbox.lower.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.upper.x, bbox.lower.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.upper.x, bbox.upper.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.lower.x, bbox.upper.y, bbox.lower.z));
  //  glEnd();
  //  glBegin(GL_LINE_LOOP);
  //  glVertex(hermes::point3(bbox.lower.x, bbox.lower.y, bbox.upper.z));
  //  glVertex(hermes::point3(bbox.upper.x, bbox.lower.y, bbox.upper.z));
  //  glVertex(hermes::point3(bbox.upper.x, bbox.upper.y, bbox.upper.z));
  //  glVertex(hermes::point3(bbox.lower.x, bbox.upper.y, bbox.upper.z));
  //  glEnd();
  //  glBegin(GL_LINES);
  //  glVertex(hermes::point3(bbox.lower.x, bbox.lower.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.lower.x, bbox.lower.y, bbox.upper.z));
  //
  //  glVertex(hermes::point3(bbox.upper.x, bbox.lower.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.upper.x, bbox.lower.y, bbox.upper.z));
  //
  //  glVertex(hermes::point3(bbox.upper.x, bbox.upper.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.upper.x, bbox.upper.y, bbox.upper.z));
  //
  //  glVertex(hermes::point3(bbox.lower.x, bbox.upper.y, bbox.lower.z));
  //  glVertex(hermes::point3(bbox.lower.x, bbox.upper.y, bbox.upper.z));
  //
  //  glEnd();
}

void draw_segment(hermes::Segment3 segment) {
  HERMES_UNUSED_VARIABLE(segment);
  //  glBegin(GL_LINES);
  //  glVertex(segment.a);
  //  glVertex(segment.b);
  //  glEnd();
}

void draw_circle(const hermes::Circle &circle,
                 const hermes::Transform2 *transform) {
  HERMES_UNUSED_VARIABLE(circle);
  HERMES_UNUSED_VARIABLE(transform);
  //  glBegin(GL_TRIANGLE_FAN);
  //  if (transform != nullptr)
  //    glVertex((*transform)(circle.c));
  //  else
  //    glVertex(circle.c);
  //  float angle = 0.0;
  //  float step = hermes::Constants::two_pi / 100.f;
  //  while (angle < hermes::Constants::two_pi + step) {
  //    hermes::vec2 pp(circle.r * cosf(angle), circle.r * sinf(angle));
  //    if (transform != nullptr)
  //      glVertex((*transform)(circle.c + pp));
  //    else
  //      glVertex(circle.c + pp);
  //    angle += step;
  //  }
  //  glEnd();
}

void draw_sphere(hermes::Sphere sphere, const hermes::Transform *transform) {
  HERMES_UNUSED_VARIABLE(sphere);
  HERMES_UNUSED_VARIABLE(transform);
  //  if (transform) {
  //    glPushMatrix();
  //    glApplyTransform(*transform);
  //  }
  //  const float vStep = hermes::Constants::pi / 20.f;
  //  const float hStep = hermes::Constants::pi / 20.f;
  //  glBegin(GL_TRIANGLES);
  //   south pole
  //  hermes::point3 pole(0.f, -sphere.r, 0.f);
  //  for (float angle = 0.f; angle < hermes::Constants::two_pi; angle += hStep)
  //  {
  //    float r = sphere.r * sinf(vStep);
  //    glVertex(sphere.c + hermes::vec3(pole));
  //    glVertex(sphere.c + hermes::vec3(pole) +
  //             r * hermes::vec3(cosf(angle), -sinf(vStep), sinf(angle)));
  //    glVertex(sphere.c + hermes::vec3(pole) +
  //             r * hermes::vec3(cosf(angle + hStep), -sinf(vStep),
  //                             sinf(angle + hStep)));
  //  }
  //   north pole
  //  pole = hermes::point3(0.f, sphere.r, 0.f);
  //  for (float angle = 0.f; angle < hermes::Constants::two_pi; angle += hStep)
  //  {
  //    float r = sphere.r * sinf(vStep);
  //    glVertex(sphere.c + hermes::vec3(pole));
  //    glVertex(sphere.c + hermes::vec3(pole) +
  //             r * hermes::vec3(cosf(angle), -sinf(vStep), sinf(angle)));
  //    glVertex(sphere.c + hermes::vec3(pole) +
  //             r * hermes::vec3(cosf(angle + hStep), -sinf(vStep),
  //                             sinf(angle + hStep)));
  //  }
  //
  //  glEnd();
  //  glBegin(GL_QUADS);
  //  for (float vAngle = vStep; vAngle <= hermes::Constants::pi - vStep;
  //       vAngle += vStep) {
  //    float r = sphere.r * sinf(vAngle);
  //    float R = sphere.r * sinf(vAngle + vStep);
  //    for (float angle = 0.f; angle < hermes::Constants::two_pi; angle +=
  //    hStep) {
  //      glVertex(sphere.c + hermes::vec3(r * cosf(angle), sphere.r *
  //      cosf(vAngle),
  //                                      r * sinf(angle)));
  //      glVertex(sphere.c + hermes::vec3(r * cosf(angle + hStep),
  //                                      sphere.r * cosf(vAngle),
  //                                      r * sinf(angle + hStep)));
  //      glVertex(sphere.c + hermes::vec3(R * cosf(angle + hStep),
  //                                      sphere.r * cosf(vAngle + vStep),
  //                                      R * sinf(angle + hStep)));
  //      glVertex(sphere.c + hermes::vec3(R * cosf(angle),
  //                                      sphere.r * cosf(vAngle + vStep),
  //                                      R * sinf(angle)));
  //    }
  //  }
  //  glEnd();
  //  if (transform)
  //    glPopMatrix();
}

//void draw_polygon(const hermes::Polygon &polygon,
//                  const hermes::Transform2 *transform) {
//  HERMES_UNUSED_VARIABLE(polygon);
//  HERMES_UNUSED_VARIABLE(transform);
  //  glBegin(GL_LINE_LOOP);
  //  for (const auto &p : polygon.vertices) {
  //    if (transform != nullptr)
  //      glVertex((*transform)(p));
  //    else
  //      glVertex(p);
  //  }
  //  glEnd();
//}

//void draw_mesh(const hermes::Mesh2D *m, const hermes::Transform2 *t) {
//  HERMES_UNUSED_VARIABLE(m);
//  HERMES_UNUSED_VARIABLE(t);
  //  glLineWidth(3.f);
  //  const hermes::RawMesh *rm = m->getMesh();
  //  glBegin(GL_LINES);
  //  for (size_t i = 0; i < rm->meshDescriptor.count; i++) {
  //    hermes::point2 a(
  //        rm->positions[rm->indices[i * rm->meshDescriptor.elementSize + 0]
  //                              .positionIndex *
  //                          2 +
  //                      0],
  //        rm->positions[rm->indices[i * rm->meshDescriptor.elementSize + 0]
  //                              .positionIndex *
  //                          2 +
  //                      1]);
  //    hermes::point2 b(
  //        rm->positions[rm->indices[i * rm->meshDescriptor.elementSize + 1]
  //                              .positionIndex *
  //                          2 +
  //                      0],
  //        rm->positions[rm->indices[i * rm->meshDescriptor.elementSize + 1]
  //                              .positionIndex *
  //                          2 +
  //                      1]);
  //    if (t)
  //      glVertex((*t)(a));
  //    else
  //      glVertex(a);
  //    if (t)
  //      glVertex((*t)(b));
  //    else
  //      glVertex(b);
  //  }
  //  glEnd();
  //  glLineWidth(1.f);
//}

void draw_vector(const hermes::point2 &p, const hermes::vec2 &v, float w,
                 float h) {
  HERMES_UNUSED_VARIABLE(p);
  HERMES_UNUSED_VARIABLE(v);
  HERMES_UNUSED_VARIABLE(w);
  HERMES_UNUSED_VARIABLE(h);
  //  glBegin(GL_LINES);
  //  glVertex(p);
  //  glVertex(p + v);
  //  glVertex(p + v);
  //  glVertex(p + v - h * v + w * v.left());
  //  glVertex(p + v);
  //  glVertex(p + v - h * v + w * v.right());
  //  glEnd();
}

} // namespace circe

#ifndef CIRCE_HELPERS_GEOMETRY_DRAWERS_H
#define CIRCE_HELPERS_GEOMETRY_DRAWERS_H

#include <circe/colors/color_palette.h>
#include <circe/gl/utils/open_gl.h>
#include <hermes/geometry/transform.h>
#include <hermes/geometry/sphere.h>
#include <hermes/geometry/segment.h>

namespace circe::gl {

void fill_box(const hermes::point2 &a, const hermes::point2 &b);

void draw_bbox(const hermes::bbox2 &bbox, float *fillColor = nullptr);

void draw_bbox(const hermes::bbox2 &bbox, const Color &edgeColor,
               const Color &fillColor);

void draw_bbox(const hermes::bbox3 &bbox);

void draw_segment(hermes::Segment3 segment);

void draw_circle(const hermes::Circle &circle,
                 const hermes::Transform2 *transform = nullptr);

void draw_sphere(hermes::Sphere sphere,
                 const hermes::Transform *transform = nullptr);

//void draw_polygon(const hermes::Polygon &polygon,
//                  const hermes::Transform2 *transform = nullptr);

//void draw_mesh(const hermes::Mesh2D *m,
//               const hermes::Transform2 *transform = nullptr);

void draw_vector(const hermes::point2 &p, const hermes::vec2 &v, float w = 0.01f,
                 float h = 0.01f);
} // namespace circe

#endif // CIRCE_HELPERS_GEOMETRY_DRAWERS_H

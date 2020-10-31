#ifndef CIRCE_HELPERS_GEOMETRY_DRAWERS_H
#define CIRCE_HELPERS_GEOMETRY_DRAWERS_H

#include <circe/colors/color_palette.h>
#include <circe/gl/utils/open_gl.h>
#include <ponos/ponos.h>

namespace circe::gl {

void fill_box(const ponos::point2 &a, const ponos::point2 &b);

void draw_bbox(const ponos::bbox2 &bbox, float *fillColor = nullptr);

void draw_bbox(const ponos::bbox2 &bbox, const Color &edgeColor,
               const Color &fillColor);

void draw_bbox(const ponos::bbox3 &bbox);

void draw_segment(ponos::Segment3 segment);

void draw_circle(const ponos::Circle &circle,
                 const ponos::Transform2 *transform = nullptr);

void draw_sphere(ponos::Sphere sphere,
                 const ponos::Transform *transform = nullptr);

void draw_polygon(const ponos::Polygon &polygon,
                  const ponos::Transform2 *transform = nullptr);

void draw_mesh(const ponos::Mesh2D *m,
               const ponos::Transform2 *transform = nullptr);

void draw_vector(const ponos::point2 &p, const ponos::vec2 &v, float w = 0.01f,
                 float h = 0.01f);
} // namespace circe

#endif // CIRCE_HELPERS_GEOMETRY_DRAWERS_H

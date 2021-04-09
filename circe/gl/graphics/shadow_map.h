/// Copyright (c) 2020, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file shadow_map.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-08-09
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_GRAPHICS_SHADOW_MAP_H
#define PONOS_CIRCE_CIRCE_GL_GRAPHICS_SHADOW_MAP_H

#include <circe/scene/light.h>
#include <circe/gl/texture/framebuffer_texture.h>
#include <circe/gl/graphics/shader.h>

namespace circe::gl {

class ShadowMap {
public:
  explicit ShadowMap(const ponos::size2 &size = ponos::size2(1024, 1024));
  ~ShadowMap();
  void setLight(const circe::Light &light);
  void render(const std::function<void(const Program &)> &f);
  void bind() const;
  [[nodiscard]] const ponos::Transform &light_transform() const;
  [[nodiscard]] const Texture &depthMap() const;
  void setLightProjection(const ponos::Transform &p) {
    projection_transform_ = p;
    light_transform_ = projection_transform_ * ponos::Transform::lookAt(ponos::point3() + 4.f * light_.direction);
  }
private:
  ponos::size2 size_;
  Framebuffer depth_buffer_;
  Texture depth_map_;
  Program program_;
  Light light_;
  ponos::Transform projection_transform_;
  ponos::Transform light_transform_;
};

}

#endif //PONOS_CIRCE_CIRCE_GL_GRAPHICS_SHADOW_MAP_H

/*
 * Copyright (c) 2019 FilipeCN
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

#ifndef CIRCE_SCENE_VOLUME_BOX_H
#define CIRCE_SCENE_VOLUME_BOX_H

#include <circe/gl/io/texture.h>
#include <circe/gl/scene/scene_object.h>

namespace circe::gl {

class VolumeBox2 : public SceneMeshObject {
public:
  VolumeBox2(u32 w, u32 h, float *data = nullptr);
  ~VolumeBox2() override;
  void draw(const CameraInterface *camera, ponos::Transform t) override;
  /// \return
  const Texture &texture() const;
  /// \return
  Texture &texture();
  /// \param data
  void update(float *data);

  float absorption = 1.f;
  ponos::vec3 light_intensity = ponos::vec3(1);
  ponos::vec2 light_position = ponos::vec2();
private:
  VolumeBox2();
  void render(GLenum cullFace);
  Texture density_texture_;
};

class VolumeBox : public SceneMeshObject {
public:
  /// \param w box width (in cells)
  /// \param h box height (in cells)
  /// \param d box depth (in cells)
  /// \param data [default = nullptr] initial data
  VolumeBox(size_t w, size_t h, size_t d, float *data = nullptr);
  ~VolumeBox() override;
  void draw(const CameraInterface *camera, ponos::Transform t) override;
  /// \return
  const Texture &texture() const;
  /// \return
  Texture &texture();
  /// \param data
  void update(float *data);

  float absorption = 1.f;
  ponos::vec3 lightIntensity = ponos::vec3(1);
  ponos::vec3 lightPos = ponos::vec3();

private:
  VolumeBox();
  void render(GLenum cullFace);
  Texture densityTexture;
};

} // namespace circe

#endif
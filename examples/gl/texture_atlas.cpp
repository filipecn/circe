/// Copyright (c) 2021, FilipeCN.
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
///\file texture_atlas.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-04-08
///
///\brief Example of a Texture atlas usage

#include <circe/circe.h>

class AtlasExample : public circe::gl::BaseApp {
public:
  AtlasExample() : BaseApp(800, 800) {
    hermes::Path assets_path(std::string(ASSETS_PATH));
    hermes::Path shaders_path(std::string(SHADERS_PATH));

    atlas_texture = circe::gl::Texture::fromFile(assets_path + "atlas.jpg");
  }

  void render(circe::CameraInterface *camera) override {
    ImGui::Begin("Texture Atlas");
    auto texture_id = atlas_texture.textureObjectId();
    ImGui::Image((void *) (intptr_t) (texture_id),
                 {static_cast<f32>( atlas_texture.size().width),
                  static_cast<f32>(atlas_texture.size().height)},
                 {0, 1}, {1, 0});
    ImGui::End();
  }

  circe::gl::Texture atlas_texture;
};

int main() {
  return AtlasExample().run();
}
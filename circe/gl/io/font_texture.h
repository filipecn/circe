// Created by filipecn on 3/29/18.
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

#ifndef CIRCE_FONT_TEXTURE_H
#define CIRCE_FONT_TEXTURE_H

#include <circe/gl/graphics/shader_manager.h>
#include <circe/gl/io/texture.h>
#include <circe/gl/scene/quad.h>
#include <memory>
#include <ponos/ponos.h>

#include <circe/gl/scene/scene_mesh.h>
#include <stb_truetype.h>

namespace circe::gl {

class FontAtlas {
public:
  struct {
    const size_t size = 40;
    const size_t atlasWidth = 1024;
    const size_t atlasHeight = 1024;
    const size_t oversampleX = 2;
    const size_t oversampleY = 2;
    const size_t firstChar = ' ';
    const size_t charCount = '~' - ' ';
    std::unique_ptr<stbtt_packedchar[]> charInfo;
    GLuint texture = 0;
  } font;
  struct Glyph {
    ponos::vec3 positions[4];
    ponos::vec2 uvs[4];
    float offsetX = 0;
    float offsetY = 0;
  };
  FontAtlas();
  /// \param path **[in]**
  void loadFont(const char *path);
  /// \brief Get the Glyph object
  /// \param character **[in]**
  /// \param offsetX **[in]**
  /// \param offsetY **[in]**
  /// \return Glyph
  [[nodiscard]] Glyph getGlyph(uint character, float offsetX, float offsetY) const;
  /// \param text **[in]**
  void setText(std::string text);
  /// \param text **[in]**
  /// \param m **[in]**
  void setText(const std::string& text, ponos::RawMesh &m) const;
  ponos::RawMeshSPtr rawMesh;
  SceneMeshSPtr mesh;
  Texture texture;
};

} // namespace circe

#endif // CIRCE_FONT_TEXTURE_H

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

#include "font_texture.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <memory>

namespace circe::gl {

void FontAtlas::loadFont(const char *path) {
  auto fontData = ponos::readFile(path);
  std::unique_ptr<uchar[]> atlasData(
      new uchar[font.atlasWidth * font.atlasHeight]);

  font.charInfo = std::make_unique<stbtt_packedchar[]>(font.charCount);

  stbtt_pack_context context;
  if (!stbtt_PackBegin(&context, atlasData.get(), font.atlasWidth,
                       font.atlasHeight, 0, 1, nullptr)) {
    std::cerr << "Failed to initialize font " << path;
    return;
  }

  stbtt_PackSetOversampling(&context, font.oversampleX, font.oversampleY);
  if (!stbtt_PackFontRange(&context, fontData.data(), 0, font.size,
                           font.firstChar, font.charCount,
                           font.charInfo.get())) {
    std::cerr << "Failed to pack font\n";
  }

  stbtt_PackEnd(&context);

  TextureParameters textureParameters;
  TextureAttributes textureAttributes;
  textureAttributes.target = GL_TEXTURE_2D;
  textureAttributes.width = font.atlasWidth;
  textureAttributes.height = font.atlasHeight;
  textureAttributes.type = GL_UNSIGNED_BYTE;
  textureAttributes.internal_format = GL_RGB;
  textureAttributes.format = GL_RED;
  texture.set(textureAttributes, textureParameters);
  texture.setTexels(atlasData.get());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenerateMipmap(GL_TEXTURE_2D);
}

FontAtlas::Glyph FontAtlas::getGlyph(uint character, float offsetX,
                                     float offsetY) const {
  stbtt_aligned_quad quad;

  stbtt_GetPackedQuad(font.charInfo.get(), font.atlasWidth, font.atlasHeight,
                      character - font.firstChar, &offsetX, &offsetY, &quad, 1);
  auto xmin = quad.x0;
  auto xmax = quad.x1;
  auto ymin = -quad.y1;
  auto ymax = -quad.y0;

  Glyph info{};
  info.offsetX = offsetX;
  info.offsetY = offsetY;
  info.positions[0] = ponos::vec3(xmin, ymin, 0);
  info.positions[1] = ponos::vec3(xmin, ymax, 0);
  info.positions[2] = ponos::vec3(xmax, ymax, 0);
  info.positions[3] = ponos::vec3(xmax, ymin, 0);
  info.uvs[0] = ponos::vec2(quad.s0, quad.t1);
  info.uvs[1] = ponos::vec2(quad.s0, quad.t0);
  info.uvs[2] = ponos::vec2(quad.s1, quad.t0);
  info.uvs[3] = ponos::vec2(quad.s1, quad.t1);

  return info;
}

void FontAtlas::setText(std::string text) {
  if (!rawMesh)
    rawMesh.reset(new ponos::RawMesh());
  rawMesh->clear();
  rawMesh->meshDescriptor.elementSize = 3;
  rawMesh->meshDescriptor.count = text.size() * 2;
  rawMesh->positionDescriptor.elementSize = 3;
  rawMesh->positionDescriptor.count = text.size() * 4;
  rawMesh->texcoordDescriptor.elementSize = 2;
  rawMesh->texcoordDescriptor.count = text.size() * 4;
  rawMesh->normalDescriptor.elementSize = 0;
  rawMesh->normalDescriptor.count = 0;

  uint16_t lastIndex = 0;
  float offsetX = 0, offsetY = 0;
  for (auto c : text) {
    const auto glyphInfo = getGlyph(c, offsetX, offsetY);
    offsetX = glyphInfo.offsetX;
    offsetY = glyphInfo.offsetY;
    for (int k = 0; k < 4; k++) {
      for (int j = 0; j < 3; j++)
        rawMesh->positions.emplace_back(glyphInfo.positions[k][j]);
      for (int j = 0; j < 2; j++)
        rawMesh->texcoords.emplace_back(glyphInfo.uvs[k][j]);
    }
    ponos::RawMesh::IndexData data{};
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex;
    rawMesh->indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 1;
    rawMesh->indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 2;
    rawMesh->indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex;
    rawMesh->indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 2;
    rawMesh->indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 3;
    rawMesh->indices.emplace_back(data);

    lastIndex += 4;
  }
  rawMesh->buildInterleavedData();
  mesh.reset(new SceneMesh(rawMesh.get()));
}

void FontAtlas::setText(const std::string &text, ponos::RawMesh &m) const {
  m.clear();
  m.meshDescriptor.elementSize = 3;
  m.meshDescriptor.count = text.size() * 2;
  m.positionDescriptor.elementSize = 3;
  m.positionDescriptor.count = text.size() * 4;
  m.texcoordDescriptor.elementSize = 2;
  m.texcoordDescriptor.count = text.size() * 4;
  m.normalDescriptor.elementSize = 0;
  m.normalDescriptor.count = 0;

  uint16_t lastIndex = 0;
  float offsetX = 0, offsetY = 0;
  for (auto c : text) {
    const auto glyphInfo = getGlyph(c, offsetX, offsetY);
    offsetX = glyphInfo.offsetX;
    offsetY = glyphInfo.offsetY;
    for (int k = 0; k < 4; k++) {
      for (int j = 0; j < 3; j++)
        m.positions.emplace_back(glyphInfo.positions[k][j]);
      for (int j = 0; j < 2; j++)
        m.texcoords.emplace_back(glyphInfo.uvs[k][j]);
    }
    ponos::RawMesh::IndexData data;
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex;
    m.indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 1;
    m.indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 2;
    m.indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex;
    m.indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 2;
    m.indices.emplace_back(data);
    data.texcoordIndex = data.normalIndex = data.positionIndex = lastIndex + 3;
    m.indices.emplace_back(data);

    lastIndex += 4;
  }
  m.buildInterleavedData();
}

FontAtlas::FontAtlas() {}
} // namespace circe
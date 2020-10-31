// Created by filipecn on 3/28/18.
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

#include "font_manager.h"

namespace circe::gl {

FontManager FontManager::instance_;

FontManager &FontManager::instance() { return instance_; }

int FontManager::loadFromFile(const char *filename) {
  instance_.fonts_.emplace_back();
  instance_.fonts_[instance_.fonts_.size() - 1].loadFont(filename);
  return instance_.fonts_.size() - 1;
}

int FontManager::loadFromFile(const std::string &filename) {
  instance_.fonts_.emplace_back();
  instance_.fonts_[instance_.fonts_.size() - 1].loadFont(filename.c_str());
  return instance_.fonts_.size() - 1;
}

void FontManager::setText(int id, const std::string &t, ponos::RawMesh &m) {
  if (id < 0 || static_cast<size_t>(id) >= instance_.fonts_.size())
    return;
  instance_.fonts_[id].setText(t, m);
}

void FontManager::bindTexture(int id, GLenum target) {
  if (id < 0 || static_cast<size_t>(id) >= instance_.fonts_.size())
    return;
  instance_.fonts_[id].texture.bind(target);
}

FontManager::FontManager() noexcept = default;

FontManager::~FontManager() = default;

} // namespace circe

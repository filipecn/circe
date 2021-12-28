// Created by filipecn on 3/3/18.
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

#include "display_renderer.h"
#include <cstddef>
#include <functional>

namespace circe::gl {

DisplayRenderer::DisplayRenderer(size_t w, size_t h) : curBuffer_(0) {
  for (auto &framebuffer_texture : framebuffer_textures_) {
    framebuffer_texture.setTarget(GL_TEXTURE_2D);
    framebuffer_texture.setType(GL_UNSIGNED_BYTE);
    framebuffer_texture.setInternalFormat(GL_RGBA8);
    framebuffer_texture.setFormat(GL_RGBA);
  }
  needsResize_ = true;
  resize(w, h);
}

void DisplayRenderer::addEffect(PostEffect *e) { effects_.emplace_back(e); }

void DisplayRenderer::process(const std::function<void()> &f) {
  resize(framebuffer_textures_->size().width, framebuffer_textures_->size().height);
  // render image to the first framebuffer
  framebuffers_[curBuffer_].render(f);
//  buffers_[curBuffer_]->render(f);
//  for (auto &effect : effects_) {
//    effect->apply(*buffers_[curBuffer_].get(),
//                  *buffers_[(curBuffer_ + 1) % 2].get());
//    curBuffer_ = (curBuffer_ + 1) % 2;
//  }
}

void DisplayRenderer::render() {
  resize(framebuffer_textures_->size().width, framebuffer_textures_->size().height);
  // render to display
  framebuffer_textures_[curBuffer_].bind(GL_TEXTURE0);
  screen.shader->begin();
  screen.shader->setUniform("tex", 0);
  screen.shader->end();
  screen.render();
}

void DisplayRenderer::resize(size_t w, size_t h) {
  if (needsResize_) {
    for (size_t i = 0; i < 2; i++) {
      framebuffer_textures_[i].resize(hermes::size2(w, h));
      framebuffers_[i].resize(hermes::size2(w, h));
      framebuffers_[i].attachTexture(framebuffer_textures_[i]);
    }
  }
  needsResize_ = false;
}

void DisplayRenderer::currentPixels(std::vector<unsigned char> &data,
                                    size_t &width, size_t &height) const {
  data = framebuffer_textures_[curBuffer_].texels();
  width = framebuffer_textures_[0].size().width;
  height = framebuffer_textures_[0].size().height;
}

} // circe namespace

// Created by filipecn on 7/10/19.
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

#include <circe/gl/ui/font_manager.h>
#include <circe/gl/ui/text_object.h>

#include <memory>

namespace circe::gl {

TextObject::TextObject(int id) : font_id_(id) {
  static int sid = ShaderManager::instance().loadFromTexts(
      "#version 440 core\n"
      "layout (location = 0) in vec3 position;"
      "layout (location = 1) in vec2 texcoord;"
      "out vec2 TexCoords;"
      "layout (location = 0) uniform mat4 model_matrix;"
      "layout (location = 1) uniform mat4 view_matrix;"
      "layout (location = 2) uniform mat4 projection_matrix;"
      "void main() {"
      "  TexCoords = texcoord;"
      "  gl_Position = projection_matrix * view_matrix * model_matrix * "
      "vec4(position, 1.0);"
      "}",
      nullptr,
      "#version 440 core\n"
      "in vec2 TexCoords;"
      "out vec4 color;"
      "layout (location = 3) uniform sampler2D text;"
      "layout (location = 4) uniform vec4 textColor;"
      "void main() {"
      "  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);"
      "  color = textColor * sampled;"
      "  color = vec4(textColor.rgb,texture(text, TexCoords).r);"
      "}");
  shader_ = createShaderProgramPtr(sid);
  shader_->addVertexAttribute("position", 0);
  shader_->addVertexAttribute("texcoord", 1);
  shader_->addUniform("model_matrix", 0);
  shader_->addUniform("view_matrix", 1);
  shader_->addUniform("projection_matrix", 2);
  shader_->addUniform("text", 3);
  shader_->addUniform("textColor", 4);
  raw_mesh_ = std::make_shared<ponos::RawMesh>();
}

void TextObject::setText(const std::string &text) {
  FontManager::instance().setText(font_id_, text, *raw_mesh_);
  mesh_.reset(new SceneMesh(raw_mesh_.get()));
}

void TextObject::draw(const CameraInterface *c, ponos::Transform t) {
  UNUSED_VARIABLE(t);
  mesh_->bind();
  mesh_->vertexBuffer()->locateAttributes(*shader_.get());
  FontManager::bindTexture(font_id_, GL_TEXTURE0);
  shader_->begin();
  shader_->setUniform("textColor", ponos::vec4(text_color.r, text_color.g,
                                               text_color.b, text_color.a));
  shader_->setUniform(
      "model_matrix",
      ponos::transpose((ponos::translate(ponos::vec3(position)) *
                        ponos::scale(text_size, text_size, text_size))
                           .matrix()));
  shader_->setUniform("view_matrix",
                      ponos::transpose(c->getViewTransform().matrix()));
  shader_->setUniform("projection_matrix",
                      ponos::transpose(c->getProjectionTransform().matrix()));
  shader_->setUniform("text", 0);
  CHECK_GL_ERRORS;
  auto ib = mesh_->indexBuffer();
  glDrawElements(ib->bufferDescriptor.element_type,
                 ib->bufferDescriptor.element_count *
                     ib->bufferDescriptor.element_size,
                 ib->bufferDescriptor.data_type, 0);
  CHECK_GL_ERRORS;
  shader_->end();
}

} // namespace circe
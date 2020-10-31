/*
 * Copyright (c) 2017 FilipeCN
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
#include <circe/gl/ui/text_renderer.h>

#include <utility>

namespace circe::gl {

TextRenderer::TextRenderer(float scale, Color c, size_t id)
    : fontId(id), textSize(scale), textColor(c) {
  dynamicScale_ = 1.f;
  dynamicColor_ = Color::Black();
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
  quad_.setShader(createShaderProgramPtr(sid));
  quad_.shader()->addVertexAttribute("position", 0);
  quad_.shader()->addVertexAttribute("texcoord", 1);
  quad_.shader()->addUniform("model_matrix", 0);
  quad_.shader()->addUniform("view_matrix", 1);
  quad_.shader()->addUniform("projection_matrix", 2);
  quad_.shader()->addUniform("text", 3);
  quad_.shader()->addUniform("textColor", 4);
}

void TextRenderer::render(std::string s, GLfloat x, GLfloat y, GLfloat scale,
                          Color c) {
  UNUSED_VARIABLE(x);
  UNUSED_VARIABLE(y);
  UNUSED_VARIABLE(scale);
  atlas.setText(std::move(s));
  atlas.mesh->bind();
  atlas.mesh->vertexBuffer()->locateAttributes(*quad_.shader().get());
  atlas.texture.bind(GL_TEXTURE0);
  quad_.shader()->begin();
  quad_.shader()->setUniform("textColor", ponos::vec4(c.r, c.g, c.b, c.a));
  quad_.shader()->setUniform(
      "model_matrix",
      ponos::transpose(ponos::translate(ponos::vec3(x, y, 0)).matrix()));
  quad_.shader()->setUniform("view_matrix", ponos::Transform().matrix());
  quad_.shader()->setUniform(
      "projection_matrix",
      ponos::transpose(ponos::ortho(0, 800, 0, 800).matrix()));
  quad_.shader()->setUniform("text", 0);
  CHECK_GL_ERRORS;
  auto ib = atlas.mesh->indexBuffer();
  glDrawElements(ib->bufferDescriptor.element_type,
                 ib->bufferDescriptor.element_count *
                     ib->bufferDescriptor.element_size,
                 ib->bufferDescriptor.data_type, nullptr);
  CHECK_GL_ERRORS;
  quad_.shader()->end();
}

void TextRenderer::render(std::string s, const ponos::point3 &p,
                          const CameraInterface *camera, GLfloat scale,
                          Color c) {
  UNUSED_VARIABLE(camera);
  atlas.setText(std::move(s));
  atlas.mesh->bind();
  atlas.mesh->vertexBuffer()->locateAttributes(*quad_.shader().get());
  atlas.texture.bind(GL_TEXTURE0);
  quad_.shader()->begin();
  quad_.shader()->setUniform("textColor", ponos::vec4(c.r, c.g, c.b, c.a));
  quad_.shader()->setUniform(
      "model_matrix", ponos::transpose((ponos::translate(ponos::vec3(p)) *
          ponos::scale(scale, scale, scale))
                                           .matrix()));
  quad_.shader()->setUniform(
      "view_matrix", ponos::transpose(camera_->getViewTransform().matrix()));
  quad_.shader()->setUniform(
      "projection_matrix",
      ponos::transpose(camera_->getProjectionTransform().matrix()));
  quad_.shader()->setUniform("text", 0);
  CHECK_GL_ERRORS;
  auto ib = atlas.mesh->indexBuffer();
  glDrawElements(ib->bufferDescriptor.element_type,
                 ib->bufferDescriptor.element_count *
                     ib->bufferDescriptor.element_size,
                 ib->bufferDescriptor.data_type, nullptr);
  CHECK_GL_ERRORS;
  quad_.shader()->end();
}

void TextRenderer::setCamera(const CameraInterface *c) {
  camera_ = c;
  usingCamera_ = true;
}

TextRenderer &TextRenderer::at(const ponos::point3 &p) {
  position_ = p;
  return *this;
}

TextRenderer &TextRenderer::at(const ponos::point2 &p) {
  position_ = ponos::point3(p.x, p.y, 0);
  return *this;
}

TextRenderer &TextRenderer::withScale(float s) {
  usingDynamicScale_ = true;
  dynamicScale_ = s;
  return *this;
}

TextRenderer &TextRenderer::withColor(Color c) {
  usingDynamicColor_ = true;
  dynamicColor_ = c;
  return *this;
}

TextRenderer &TextRenderer::operator<<(TextRenderer &tr) { return tr; }

TextRenderer::TextRenderer(const std::string &filename) : TextRenderer() {
  atlas.loadFont(filename.c_str());
}

} // namespace circe

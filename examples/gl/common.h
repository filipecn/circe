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
///\file common.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-03-28
///
///\brief

#ifndef CIRCE_EXAMPLES_GL_COMMON_H
#define CIRCE_EXAMPLES_GL_COMMON_H

#include <circe/circe.h>

struct alignas(16) vec3_16 {
  vec3_16() = default;
  vec3_16(float x, float y, float z) : x(x), y(y), z(z) {}
  vec3_16 &operator=(const circe::Color &color) {
    x = color.r;
    y = color.g;
    z = color.b;
    return *this;
  }
  float x{0};
  float y{0};
  float z{0};
};

struct alignas(16) PBR_UB {
  vec3_16 albedo;
  float metallic{};
  float roughness{};
  float ao{};
};

/// Holds a framebuffer object to render into an output texture by processing
/// an input texture
class BufferView {
public:
  static BufferView depthBufferView() {
    // shader
    std::vector<circe::gl::Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, vert_source);
    shaders.emplace_back(GL_FRAGMENT_SHADER,
                         (hermes::Str(frag_base) +
                             "    FragColor = vec4(vec3(texture(tex, fuv).r), 1.0);}").str());

    BufferView bf;
    if (!bf.program.link(shaders))
      hermes::Log::error("shader error: {}", bf.program.err);
    bf.program.use();
    bf.program.setUniform("tex", 0);
    return bf;
  }

  static BufferView vec3BufferView(bool normalize_data = false) {
    // shader
    std::vector<circe::gl::Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, vert_source);
    if (normalize_data)
      shaders.emplace_back(GL_FRAGMENT_SHADER,
                           (hermes::Str(frag_base) +
                               "    vec3 v = normalize(texture(tex, fuv).rgb);\n"
                               "    v += vec3(1.0,1.0,1.0);\n"
                               "    v /= vec3(2.0,2.0,2.0);\n"
                               "    FragColor = vec4(v, 1.0);}").str());
    else
      shaders.emplace_back(GL_FRAGMENT_SHADER,
                           (hermes::Str(frag_base) +
                               "    vec3 v = texture(tex, fuv).rgb;\n"
                               "    FragColor = vec4(v, 1.0);}").str());
    BufferView bf;
    bf.program.link(shaders);
    bf.program.use();
    bf.program.setUniform("tex", 0);
    return bf;
  }

  static BufferView vec4BufferView() {
    // shader
    std::vector<circe::gl::Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, vert_source);
    shaders.emplace_back(GL_FRAGMENT_SHADER,
                         (hermes::Str(frag_base) +
                             "    FragColor = texture(tex, fuv).rgba;}").str());
    BufferView bf;
    bf.program.link(shaders);
    bf.program.use();
    bf.program.setUniform("tex", 0);
    return bf;
  }

  static BufferView componentBufferView(int c = 3) {
    // shader
    std::vector<circe::gl::Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, vert_source);

    char component_names[5] = "rgba";
    shaders.emplace_back(GL_FRAGMENT_SHADER,
                         (hermes::Str(frag_base) <<
                                                 "    float v = texture(tex, fuv)." << component_names[c]
                                                 << ";\nFragColor = vec4(v,v,v,1.0);}").str());
    BufferView bf;
    bf.program.link(shaders);
    bf.program.use();
    bf.program.setUniform("tex", 0);
    return bf;
  }

  BufferView() {
    // setup screen quad
    screen_quad = circe::Shapes::box({{-1, -1}, {1, 1}}, circe::shape_options::uv);
    // tex parameters
    texture_parameters[GL_TEXTURE_MIN_FILTER] = GL_NEAREST;
    texture_parameters[GL_TEXTURE_MAG_FILTER] = GL_NEAREST;
  }

  /// \param input
  /// \param rect imgui size
  void render(const circe::gl::Texture &input, const hermes::size2 &rect = {256, 256}) {
    if (input.size().slice(0, 1) != buffer_texture.size().slice(0, 1))
      buffer_texture.resize(input.size());
    if (framebuffer.size() != buffer_texture.size().slice(0, 1))
      framebuffer.resize(input.size());
    buffer_texture.bind();
    texture_parameters.apply();
    framebuffer.attachTexture(buffer_texture);
    framebuffer.render([&]() {
      input.bind(GL_TEXTURE0);
      program.use();
      screen_quad.draw();
    });
    auto texture_id = buffer_texture.textureObjectId();
    ImGui::Image((void *) (intptr_t) (texture_id), {256, 256},
                 {0, 1}, {1, 0});
  }

  auto textureId() const { return buffer_texture.textureObjectId(); }

private:
  circe::gl::Texture::View texture_parameters;
  circe::gl::Texture buffer_texture;
  circe::gl::Program program;
  circe::gl::Framebuffer framebuffer;
  circe::gl::SceneModel screen_quad;
  static const constexpr auto vert_source = "#version 440 core\n"
                                            "layout(location = 0) in vec3 position;\n"
                                            "layout(location = 1) in vec2 uv;\n"
                                            "out vec2 fuv;\n"
                                            "void main() {\n"
                                            "    gl_Position = vec4(position, 1.0);\n"
                                            "    fuv = uv;}";
  static const constexpr auto frag_base = "#version 440 core\n"
                                          "out vec4 FragColor;\n"
                                          "in vec2 fuv;\n"
                                          "uniform sampler2D tex;\n"
                                          "void main(){\n";
};

#endif //CIRCE_EXAMPLES_GL_COMMON_H

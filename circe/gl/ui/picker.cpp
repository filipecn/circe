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
///\file picker.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-13
///
///\brief

#include <circe/gl/ui/picker.h>

namespace circe::gl {

const char *object_pick_vs =
    "#version 440 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 3) uniform mat4 projection;\n"
    "layout(location = 4) uniform mat4 model;\n"
    "layout(location = 5) uniform mat4 view;\n"
    "void main() {\n"
    "  gl_Position = projection * view * model * vec4(position, 1.0);\n"
    "}";

const char *object_pick_fs =
    "#version 440 core\n"
    "layout(location = 0) out unsigned int result;"
    "uniform int object_id;"
    "void main() {\n"
    "  result = object_id;"
    "}";

const char *mesh_pick_vs =
    "#version 440 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec3 barycentric;\n"
    "layout(location = 3) uniform mat4 projection;\n"
    "layout(location = 4) uniform mat4 model;\n"
    "layout(location = 5) uniform mat4 view;\n"
    "out vec3 vbc;\n"
    "void main() {\n"
    "  vbc = barycentric;\n"
    "  gl_Position = projection * view * model * vec4(position, 1.0);\n"
    "}";

const char *mesh_pick_fs =
    "#version 440 core\n"
    "in vec3 vbc;\n"
    "layout(location = 0) out int p_object_id;\n"
    "layout(location = 1) out int primitive_id;\n"
    "layout(location = 2) out int edge_id;\n"
    "uniform int object_id;\n"
    "uniform float line_width;\n"
    "uniform float vertex_width;\n"
    "void main() {\n"
    "  p_object_id = object_id;\n"
    "  primitive_id = gl_PrimitiveID;\n"
    "  edge_id = 0;\n"
    "  vec3 d = fwidth(vbc);\n"
    "  vec3 f = step(d * line_width, vbc);\n"
    "  if(min(min(f.x, f.y), f.z) < 1) {\n"
    "   edge_id = 30;\n"
    "   if(f.x < f.y && f.x < f.z)\n"
    "     edge_id = 10;\n"
    "   else if(f.y < f.z && f.y < f.z)\n"
    "     edge_id = 20;\n"
    "}\n"
    "  f = step(d * vertex_width, vbc);\n"
    "  if(min(min(f.x, f.y), f.z) < 1) {\n"
    "   int vertex_id = 0;\n"
    "   if(vbc.x > 0.9)"
    "     vertex_id = 1;\n"
    "   else if(vbc.y > 0.9)\n"
    "     vertex_id = 2;\n"
    "   else if(vbc.z > 0.9)\n"
    "     vertex_id = 3;\n"
    "   edge_id += vertex_id;\n"
    "}\n"
    "}";

Picker::Picker() {
  // compile programs
  program_.attach(Shader(GL_VERTEX_SHADER, object_pick_vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, object_pick_fs));
  if (!program_.link())
    HERMES_LOG_ERROR("Failed to compile picker shader: " + program_.err)
}

Picker::~Picker() = default;

void Picker::pick(const circe::CameraInterface *camera,
                  const hermes::index2 &pick_position,
                  const std::function<void(const Program &)> &f) {
  // check pick position
  if (!(pick_position < t_object_id_.size().slice()) || !(pick_position >= hermes::index2(0, 0)))
    return;

  glEnable(GL_SCISSOR_TEST);
  glScissor(static_cast<int>(pick_position.i - 1), static_cast<int>(pick_position.j - 1), 3, 3);
  t_object_id_.bind();
  fbo_.render([&]() {
    program_.use();
    program_.setUniform("projection", camera->getProjectionTransform());
    program_.setUniform("view", camera->getViewTransform());
    f(program_);
  });
  glDisable(GL_SCISSOR_TEST);
  // get result
  // TODO I should get just the subregion! but it is not working for some reason....
  //  auto data = t_object_id_.texels({static_cast<int>(pick_position.x - 1),
  //                                      static_cast<int>(pick_position.y - 1)}, {3, 3});
  auto data = t_object_id_.texels();
  // get mouse texel
  picked_index = reinterpret_cast<u32 *>(data.data())[(int) pick_position.j * t_object_id_.size().width
      + (int) pick_position.i];
}

void Picker::setResolution(const hermes::size2 &resolution_in_pixels) {
  fbo_.resize(resolution_in_pixels);
  fbo_.clear_color = {0, 0, 0, 0};
  circe::gl::Texture::Attributes attributes = {
      .size_in_texels = {resolution_in_pixels.width, resolution_in_pixels.height, 1},
      .internal_format = GL_R32UI,
      .format = GL_RED_INTEGER,
      .type = GL_UNSIGNED_INT,
      .target = GL_TEXTURE_2D,
  };
  circe::gl::Texture::View parameters(GL_TEXTURE_2D);
  parameters[GL_TEXTURE_MIN_FILTER] = GL_NEAREST;
  parameters[GL_TEXTURE_MAG_FILTER] = GL_NEAREST;
  t_object_id_.set(attributes);
  t_object_id_.bind();
  parameters.apply();
  fbo_.attachTexture(t_object_id_, GL_COLOR_ATTACHMENT0);
  fbo_.setOutputBuffers({GL_COLOR_ATTACHMENT0});
}

const circe::gl::Texture &Picker::result() const {
  return t_object_id_;
}

MeshPicker::MeshPicker() {
  program_.destroy();
  program_.attach(Shader(GL_VERTEX_SHADER, mesh_pick_vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, mesh_pick_fs));
  if (!program_.link())
    HERMES_LOG_ERROR("Failed to compile picker shader: " + program_.err)
}

MeshPicker::~MeshPicker() = default;

void MeshPicker::pick(const circe::CameraInterface *camera,
                      const hermes::index2 &pick_position,
                      const std::function<void(const Program &)> &f) {
  // check pick position
  if (!(pick_position < t_object_id_.size().slice()) || !(pick_position >= hermes::index2(0, 0)))
    return;

  glEnable(GL_SCISSOR_TEST);
  glScissor(static_cast<int>(pick_position.i - 1), static_cast<int>(pick_position.j - 1), 3, 3);
  t_object_id_.bind();
  t_primitive_id_.bind();
  t_barycentric_.bind();
  fbo_.render([&]() {
    program_.use();
    program_.setUniform("projection", camera->getProjectionTransform());
    program_.setUniform("view", camera->getViewTransform());
    program_.setUniform("line_width", edge_width);
    program_.setUniform("vertex_width", vertex_width);
    f(program_);
  });
  glDisable(GL_SCISSOR_TEST);
  // get result
  // TODO I should get just the subregion! but it is not working for some reason....
  //  auto data = t_object_id_.texels({static_cast<int>(pick_position.x - 1),
  //                                      static_cast<int>(pick_position.y - 1)}, {3, 3});
  {
    auto data = t_object_id_.texels();
    // get mouse texel
    picked_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_object_id_.size().width + pick_position.i];
  }
  {
    auto data = t_primitive_id_.texels();
    // get mouse texel
    picked_primitive_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_primitive_id_.size().width
        + pick_position.i];
  }
  {
    auto data = t_edge_id_.texels();
    // get mouse texel
    picked_edge_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_edge_id_.size().width
        + pick_position.i];
    picked_vertex_index = picked_edge_index % 10;
    picked_edge_index /= 10;
  }
  {
//    auto data = t_barycentric_.texels();
    // get mouse texel
//    picked_barycentric_coordinates = {
//        reinterpret_cast<f32 *>(data.data())[pick_position.j * t_barycentric_.size().width * 3
//            + pick_position.i * 3 + 0],
//        reinterpret_cast<f32 *>(data.data())[pick_position.j * t_barycentric_.size().width * 3
//            + pick_position.i * 3 + 1],
//        reinterpret_cast<f32 *>(data.data())[pick_position.j * t_barycentric_.size().width * 3
//            + pick_position.i * 3 + 2],
//    };
  }
}

void MeshPicker::setResolution(const hermes::size2 &resolution_in_pixels) {
  Picker::setResolution(resolution_in_pixels);
  // setup
  circe::gl::Texture::Attributes attributes = {
      .size_in_texels = {resolution_in_pixels.width, resolution_in_pixels.height, 1},
      .internal_format = GL_R32UI,
      .format = GL_RED_INTEGER,
      .type = GL_UNSIGNED_INT,
      .target = GL_TEXTURE_2D,
  };

  circe::gl::Texture::View parameters(GL_TEXTURE_2D);
  parameters[GL_TEXTURE_MIN_FILTER] = GL_NEAREST;
  parameters[GL_TEXTURE_MAG_FILTER] = GL_NEAREST;

  t_object_id_.bind();
  parameters.apply();
  fbo_.attachTexture(t_object_id_, GL_COLOR_ATTACHMENT0);

  t_primitive_id_.set(attributes);
  t_primitive_id_.bind();
  parameters.apply();
  fbo_.attachTexture(t_primitive_id_, GL_COLOR_ATTACHMENT1);

  t_edge_id_.set(attributes);
  t_edge_id_.bind();
  parameters.apply();
  fbo_.attachTexture(t_edge_id_, GL_COLOR_ATTACHMENT2);

//  attributes.internal_format = GL_RGB32F;
//  attributes.format = GL_RGB;
//  attributes.type = GL_FLOAT;
//  t_barycentric_.set(attributes);
//  t_barycentric_.bind();
//  parameters.apply();
//  fbo_.attachTexture(t_barycentric_, GL_COLOR_ATTACHMENT2);

  fbo_.setOutputBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
}

}
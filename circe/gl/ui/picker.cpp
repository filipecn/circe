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
#include <hermes/common/profiler.h>

namespace circe::gl {

const char *mesh_pick_rt_vs =
    "#version 450\n"
    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 0) out vec3 v_world_pos;\n"
    "layout (location = 1) out vec3 v_view_vector;\n"
    "layout(location = 2) uniform vec3 camera_pos;\n"
    "layout(location = 3) uniform mat4 projection;\n"
    "layout(location = 4) uniform mat4 model;\n"
    "layout(location = 5) uniform mat4 view;\n"
    "void main() {\n"
    "  vec3 world_pos = (vec4(a_pos, 1) * model).xyz;\n"
    "  gl_Position = projection * view * vec4(world_pos, 1.0);\n"
    "  v_world_pos = world_pos;\n"
    "  v_view_vector = world_pos - camera_pos.xyz;\n"
    "}";

const char *mesh_pick_rt_fs =
    "#version 450\n"
    "struct VertexPacked\n"
    "{\n"
    " vec3 p;\n"
    "};"
    "layout (std430, binding = 3) readonly buffer VertexBuffer {\n"
    " VertexPacked g_vertices[];\n"
    "};\n"
    "layout (std430, binding = 4) readonly buffer IndexBuffer {\n"
    " uint g_indices[];\n"
    "};"
    "vec3 intersectRayTri(vec3 rayOrigin, vec3 rayDirection, vec3 v0, vec3 v1, vec3 v2){\n"
    "  vec3 e0 = v1 - v0;\n"
    "  vec3 e1 = v2 - v0;\n"
    "  vec3 s1 = cross(rayDirection, e1);\n"
    "  float  invd = 1.0 / (dot(s1, e0));\n"
    "  vec3 d = rayOrigin - v0;\n"
    "  float  b1 = dot(d, s1) * invd;\n"
    "  vec3 s2 = cross(d, e0);\n"
    "  float  b2 = dot(rayDirection, s2) * invd;\n"
    "  float temp = dot(e1, s2) * invd;\n"
    "  return vec3(1.0 - b1 - b2, b1, b2);\n"
    "}\n"
    "layout (location = 0) in flat vec3 v_world_pos;\n"
    "layout (location = 1) in vec3 v_view_vector;\n"
    "layout(location = 2) uniform vec3 camera_pos;\n"
    "layout(location = 4) uniform mat4 model;\n"
    "layout(location = 0) out int p_object_id;\n"
    "layout(location = 1) out int primitive_id;\n"
    "layout(location = 2) out int edge_id;\n"
    "uniform int object_id;\n"
    "uniform float line_width;\n"
    "uniform float vertex_width;\n"
    "void main(){\n"
    "  uint index1 = g_indices[gl_PrimitiveID*3+1];\n"
    "  uint index2 = g_indices[gl_PrimitiveID*3+2];\n"
    "  vec3 vertex1 = g_vertices[index1].p;\n"
    "  vec3 vertex2 = g_vertices[index2].p;\n"
    "  vec3 vbc = intersectRayTri(camera_pos,normalize(v_view_vector),"
    "                             v_world_pos,"
    "                             (vec4(vertex1, 1) * model).xyz,"
    "                             (vec4(vertex2, 1) * model).xyz);\n"
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
    "  }\n"
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
    "  }\n"
    "}";

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
    "  }\n"
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
    "  }\n"
    "}";

const char *instance_pick_vs =
    "#version 440 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 2) in mat4 transform_matrix\n;"
    "layout(location = 3) uniform mat4 projection;\n"
    "layout(location = 4) uniform mat4 model;\n"
    "layout(location = 5) uniform mat4 view;\n"
    "out flat int instance;"
    "void main() {\n"
    "  gl_Position = projection * view * model * transform_matrix * vec4(position, 1.0);"
    "  instance = gl_InstanceID;"
    "}";

const char *instance_pick_fs =
    "#version 440 core\n"
    "layout(location = 0) out unsigned int result;"
    "in flat int instance;"
    "void main() {\n"
    "  result = instance + 1;"
    "}";

Picker::Picker() {
  // compile programs
  program_.attach(Shader(GL_VERTEX_SHADER, object_pick_vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, object_pick_fs));
  if (!program_.link())
  HERMES_LOG_ERROR("Failed to compile picker shader: {}", program_.err);
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
    program_.setUniform("model", hermes::Transform());
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
  HERMES_LOG_ERROR("Failed to compile picker shader: {}", program_.err);
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
  std::vector<unsigned char> data(4 * 3 * 3);
  {
    HERMES_PROFILE_SCOPE("pick primitive");
//    auto data = t_primitive_id_.texels();
//    auto data = t_primitive_id_.texels({static_cast<int>(pick_position.i - 1),
//                                        static_cast<int>(pick_position.j - 1)}, {3, 3});
    t_primitive_id_.texels({static_cast<int>(pick_position.i - 1),
                            static_cast<int>(pick_position.j - 1)}, {3, 3}, &data[0]);
    // get mouse texel
//    picked_primitive_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_primitive_id_.size().width
//        + pick_position.i];
    if (!data.empty())
      picked_primitive_index = reinterpret_cast<u32 *>(data.data())[4];
  }
  {
    HERMES_PROFILE_SCOPE("pick object");
    // TODO I should get just the subregion! but it is not working for some reason....
//    auto data = t_object_id_.texels({static_cast<int>(pick_position.i - 1),
//                                     static_cast<int>(pick_position.j - 1)}, {3, 3});
    HERMES_PROFILE_START_BLOCK("texels");
    t_object_id_.texels({static_cast<int>(pick_position.i - 1),
                         static_cast<int>(pick_position.j - 1)}, {3, 3}, &data[0]);
    HERMES_PROFILE_END_BLOCK
//    auto data = t_object_id_.texels();
    // get mouse texel
//    picked_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_object_id_.size().width + pick_position.i];
    if (!data.empty())
      picked_index = reinterpret_cast<u32 *>(data.data())[4];
  }

  {
    HERMES_PROFILE_SCOPE("pick edge");
//    auto data = t_edge_id_.texels();
//    auto data = t_edge_id_.texels({static_cast<int>(pick_position.i - 1),
//                                   static_cast<int>(pick_position.j - 1)}, {3, 3});
    t_edge_id_.texels({static_cast<int>(pick_position.i - 1),
                       static_cast<int>(pick_position.j - 1)}, {3, 3}, &data[0]);
    // get mouse texel
//    picked_edge_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_edge_id_.size().width
//        + pick_position.i];
    if (!data.empty()) {
      picked_edge_index = reinterpret_cast<u32 *>(data.data())[4];
      picked_vertex_index = picked_edge_index % 10;
      picked_edge_index /= 10;
    }
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

InstancePicker::InstancePicker() {
  program_.destroy();
  program_.attach(Shader(GL_VERTEX_SHADER, instance_pick_vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, instance_pick_fs));
  HERMES_ASSERT_WITH_LOG(program_.link(), program_.err);
}

InstancePicker::~InstancePicker() = default;

void InstancePicker::pick(const circe::CameraInterface *camera,
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
    program_.setUniform("model", hermes::Transform());
    f(program_);
  });
  glDisable(GL_SCISSOR_TEST);
  HERMES_PROFILE_SCOPE("pick data");
  // get result
  // TODO I should get just the subregion! but it is not working for some reason....
  //  auto data = t_object_id_.texels({static_cast<int>(pick_position.x - 1),
  //                                      static_cast<int>(pick_position.y - 1)}, {3, 3});
  auto data = t_object_id_.texels();
  // get mouse texel
  picked_index = reinterpret_cast<i32 *>(data.data())[(int) pick_position.j * t_object_id_.size().width
      + (int) pick_position.i] - 1;
}

RTMeshPicker::RTMeshPicker() {
  program_.destroy();
  program_.attach(Shader(GL_VERTEX_SHADER, mesh_pick_rt_vs));
  program_.attach(Shader(GL_FRAGMENT_SHADER, mesh_pick_rt_fs));
  if (!program_.link())
  HERMES_LOG_ERROR("Failed to compile rt picker shader: {}", program_.err);
}

RTMeshPicker::~RTMeshPicker() = default;

HeResult RTMeshPicker::setModel(SceneModel *model) {
  model_ = model;
  vertex_ssbo_ = ShaderStorageBuffer::fromVertexBuffer(model->vertexBuffer());
  vertex_ssbo_.setBindingIndex(3);
  index_ssbo_ = ShaderStorageBuffer::fromIndexBuffer(model->indexBuffer());
  index_ssbo_.setBindingIndex(4);
  return HeResult::SUCCESS;
}

void RTMeshPicker::pick(const circe::CameraInterface *camera,
                      const hermes::index2 &pick_position) {
  // check pick position
  if (!(pick_position < t_object_id_.size().slice()) || !(pick_position >= hermes::index2(0, 0)))
    return;

  glEnable(GL_SCISSOR_TEST);
  glScissor(static_cast<int>(pick_position.i - 1), static_cast<int>(pick_position.j - 1), 3, 3);
  t_object_id_.bind();
  t_primitive_id_.bind();
  t_barycentric_.bind();
  index_ssbo_.bind();
  vertex_ssbo_.bind();
  fbo_.render([&]() {
    program_.use();
    program_.setUniform("projection", camera->getProjectionTransform());
    program_.setUniform("view", camera->getViewTransform());
    program_.setUniform("line_width", edge_width);
    program_.setUniform("vertex_width", vertex_width);
    program_.setUniform("camera_pos", camera->getPosition());
    model_->draw();
  });
  glDisable(GL_SCISSOR_TEST);
  // get result
  std::vector<unsigned char> data(4 * 3 * 3);
  {
    HERMES_PROFILE_SCOPE("pick primitive");
//    auto data = t_primitive_id_.texels();
//    auto data = t_primitive_id_.texels({static_cast<int>(pick_position.i - 1),
//                                        static_cast<int>(pick_position.j - 1)}, {3, 3});
    t_primitive_id_.texels({static_cast<int>(pick_position.i - 1),
                            static_cast<int>(pick_position.j - 1)}, {3, 3}, &data[0]);
    // get mouse texel
//    picked_primitive_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_primitive_id_.size().width
//        + pick_position.i];
    if (!data.empty())
      picked_primitive_index = reinterpret_cast<u32 *>(data.data())[4];
  }
  {
    HERMES_PROFILE_SCOPE("pick object");
    // TODO I should get just the subregion! but it is not working for some reason....
//    auto data = t_object_id_.texels({static_cast<int>(pick_position.i - 1),
//                                     static_cast<int>(pick_position.j - 1)}, {3, 3});
    HERMES_PROFILE_START_BLOCK("texels");
    t_object_id_.texels({static_cast<int>(pick_position.i - 1),
                         static_cast<int>(pick_position.j - 1)}, {3, 3}, &data[0]);
    HERMES_PROFILE_END_BLOCK
//    auto data = t_object_id_.texels();
    // get mouse texel
//    picked_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_object_id_.size().width + pick_position.i];
    if (!data.empty())
      picked_index = reinterpret_cast<u32 *>(data.data())[4];
  }

  {
    HERMES_PROFILE_SCOPE("pick edge");
//    auto data = t_edge_id_.texels();
//    auto data = t_edge_id_.texels({static_cast<int>(pick_position.i - 1),
//                                   static_cast<int>(pick_position.j - 1)}, {3, 3});
    t_edge_id_.texels({static_cast<int>(pick_position.i - 1),
                       static_cast<int>(pick_position.j - 1)}, {3, 3}, &data[0]);
    // get mouse texel
//    picked_edge_index = reinterpret_cast<u32 *>(data.data())[pick_position.j * t_edge_id_.size().width
//        + pick_position.i];
    if (!data.empty()) {
      picked_edge_index = reinterpret_cast<u32 *>(data.data())[4];
      picked_vertex_index = picked_edge_index % 10;
      picked_edge_index /= 10;
    }
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

}
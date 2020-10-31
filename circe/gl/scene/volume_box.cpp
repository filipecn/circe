/*
 * Copyright (c) 2019 FilipeCN
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

#include <circe/gl/scene/volume_box.h>

namespace circe::gl {

VolumeBox2::VolumeBox2(u32 w, u32 h, float *data) : VolumeBox2() {
  TextureAttributes ta;
  ta.width = w;
  ta.height = h;
  ta.internal_format = GL_RED;
  ta.format = GL_RED;
  ta.type = GL_FLOAT;
  ta.target = GL_TEXTURE_2D;
  TextureParameters tp;
  tp.target = GL_TEXTURE_2D;
  tp[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
  tp[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
  tp[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_BORDER;
  tp[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_BORDER;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  density_texture_.set(ta, tp);
  density_texture_.setTexels(data);
}

VolumeBox2::VolumeBox2() {
  const char *vs = "#version 440 core\n"
                   "layout (location = 0) in vec2 position;"
                   "layout (location = 1) in vec2 texcoord;"
                   "layout (location = 2) uniform mat4 model_view_matrix;"
                   "layout (location = 3) uniform mat4 projection_matrix;"
                   "out vec2 tex;"
                   "void main() {"
                   "  gl_Position = projection_matrix * model_view_matrix * "
                   "vec4(position, 0.0, 1.0);"
                   "  tex = texcoord;"
                   "}";
  const char *fs = "#version 440 core\n"
                   "layout(location = 0) out vec4 fragColor;"
                   //      "layout(location = 4) uniform vec3 cameraPosition;"
                   "layout(location = 5) uniform sampler2D g_densityTex;"
                   //      "layout(location = 6) uniform vec2 g_lightPos;"
                   //      "layout(location = 7) uniform vec3 g_lightIntensity;"
                   "layout(location = 8) uniform float g_absorption;"
                   "in vec2 tex;"
                   "void main() {\n"
                   // diagonal of the cube
                   "const float maxDist = sqrt(2.0);\n"
                   "const int numSamples = 128;\n"
                   "const float scale = maxDist/float(numSamples);\n"
                   "const int numLightSamples = 32;\n"
                   "const float lscale = maxDist / float(numLightSamples);\n"
                   // assume all coordinates are in texture space
                   "vec2 pos = tex;\n"
                   // sample density
                   "float density = texture(g_densityTex, pos).x;\n"
                   "if(pos.x < 0.0 || pos.y < 0.0 || pos.x > 1.0 || "
                   "pos.y > 1.0) density = 0.;\n"
                   "float T = 1.0;\n"
                   // skip empty space
                   "    if (density > 0.0) {"
                   // attenuate ray-throughput
                   "        T *= 1.0-density*scale*g_absorption;}\n"

                   "fragColor.xyz = vec3(density,density,density);\n"
                   "fragColor.w = 1.0;// - T;\n"
                   //      "fragColor = vec4(0,1,0,1);"
                   "}";
  shader_ = createShaderProgramPtr(vs, nullptr, fs);
  shader_->addVertexAttribute("position", 0);
  shader_->addVertexAttribute("texcoord", 1);
  shader_->addUniform("model_view_matrix", 2);
  shader_->addUniform("projection_matrix", 3);
  //  shader_->addUniform("cameraPosition", 4);
  shader_->addUniform("g_densityTex", 5);
  //  shader_->addUniform("g_lightPos", 6);
  //  shader_->addUniform("g_lightIntensity", 7);
  shader_->addUniform("g_absorption", 8);
  mesh_ = createSceneMeshPtr(ponos::RawMeshes::quad(ponos::Transform(), true));
}

VolumeBox2::~VolumeBox2() = default;

void VolumeBox2::draw(const CameraInterface *camera, ponos::Transform t) {
  UNUSED_VARIABLE(t);
  density_texture_.bind(GL_TEXTURE0);
  shader_->begin();
  shader_->setUniform("model_view_matrix",
                      ponos::transpose(camera->getViewTransform().matrix()));
  shader_->setUniform(
      "projection_matrix",
      ponos::transpose(camera->getProjectionTransform().matrix()));
  CHECK_GL_ERRORS;
  //  shader_->setUniform("cameraPosition", camera->getPosition());
  shader_->setUniform("g_densityTex", 0);
  //  shader_->setUniform("g_lightPos", light_position);
  CHECK_GL_ERRORS;
  //  shader_->setUniform("g_lightIntensity", light_intensity);
  shader_->setUniform("g_absorption", absorption);
  CHECK_GL_ERRORS;
  mesh_->bind();
  mesh_->vertexBuffer()->locateAttributes(*shader_.get());
  render(GL_FRONT);
  mesh_->unbind();
  shader_->end();
  CHECK_GL_ERRORS;
}

void VolumeBox2::render(GLenum cullFace) {
  UNUSED_VARIABLE(cullFace);
  //  glEnable(GL_DEPTH_TEST);
  //  glEnable(GL_CULL_FACE);
  //  glFrontFace(GL_CCW);
  //  glCullFace(cullFace);
  glDrawElements(mesh_->indexBuffer()->bufferDescriptor.element_type,
                 mesh_->indexBuffer()->bufferDescriptor.element_count *
                     mesh_->indexBuffer()->bufferDescriptor.element_size,
                 GL_UNSIGNED_INT, nullptr);
}

const Texture &VolumeBox2::texture() const { return density_texture_; }

Texture &VolumeBox2::texture() { return density_texture_; }

void VolumeBox2::update(float *data) {
  density_texture_.setTexels(reinterpret_cast<unsigned char *>(data));
}

VolumeBox::VolumeBox() {
  const char *vs = "#version 440 core\n"
                   "layout (location = 0) in vec3 position;"
                   "layout (location = 1) in vec3 texcoord;"
                   "layout (location = 2) uniform mat4 model_view_matrix;"
                   "layout (location = 3) uniform mat4 projection_matrix;"
                   "out vec3 tex;"
                   "void main() {"
                   "  gl_Position = projection_matrix * model_view_matrix * "
                   "vec4(position, 1.0);"
                   "  tex = texcoord;"
                   "}";
  const char *fs =
      "#version 440 core\n"
      "layout(location = 0) out vec4 fragColor;"
      "layout(location = 4) uniform vec3 cameraPosition;"
      "layout(location = 5) uniform sampler3D g_densityTex;"
      "layout(location = 6) uniform vec3 g_lightPos;"
      "layout(location = 7) uniform vec3 g_lightIntensity;"
      "layout(location = 8) uniform float g_absorption;"
      "in vec3 tex;"
      "void main() {"
      // diagonal of the cube
      "const float maxDist = sqrt(3.0);"
      "const int numSamples = 128;"
      "const float scale = maxDist/float(numSamples);"
      "const int numLightSamples = 32;"
      "const float lscale = maxDist / float(numLightSamples);"
      // assume all coordinates are in texture space
      "vec3 pos = tex;"
      "vec3 eyeDir = normalize(pos-cameraPosition)*scale;"
      // transmittance
      "float T = 1.0;"
      // in-scattered radiance
      "vec3 Lo = vec3(0.0);"
      "for (int i=0; i < numSamples; ++i) {"
      // sample density
      "    float density = texture(g_densityTex, pos).x;"
      "     if(pos.x < 0.0 || pos.y < 0.0 || pos.z < 0.0 || pos.x > 1.0 || "
      "pos.y > 1.0 || pos.z > 1.0) density = 0.;"
      // skip empty space
      "    if (density > 0.0) {"
      // attenuate ray-throughput
      "        T *= 1.0-density*scale*g_absorption;"
      "        if (T <= 0.01)"
      "            break;"
      // point light dir in texture space
      "        vec3 lightDir = normalize(g_lightPos-pos)*lscale;"
      // sample light
      // transmittance along light ray
      "        float Tl = 1.0;  "
      "        vec3 lpos = pos + lightDir;"
      "        for (int s=0; s < numLightSamples; ++s) {"
      "            float ld = texture(g_densityTex, lpos).x;"
      "     if(lpos.x < 0.0 || lpos.y < 0.0 || lpos.z < 0.0 || lpos.x > 1.0 || "
      "lpos.y > 1.0 || lpos.z > 1.0) ld = 0.;"
      "            Tl *= 1.0-g_absorption*lscale*ld;"
      "            if (Tl <= 0.01)"
      "                break;"
      "            lpos += lightDir;"
      "        }"
      "        vec3 Li = g_lightIntensity*Tl;"
      "        Lo += Li*T*density*scale;"
      "    }"
      "    pos += eyeDir;"
      "}"
      "fragColor.xyz = Lo;"
      "fragColor.w = 1.0 - T;"
      "}";
  shader_ = createShaderProgramPtr(vs, nullptr, fs);
  shader_->addVertexAttribute("position", 0);
  shader_->addVertexAttribute("texcoord", 1);
  shader_->addUniform("model_view_matrix", 2);
  shader_->addUniform("projection_matrix", 3);
  shader_->addUniform("cameraPosition", 4);
  shader_->addUniform("g_densityTex", 5);
  shader_->addUniform("g_lightPos", 6);
  shader_->addUniform("g_lightIntensity", 7);
  shader_->addUniform("g_absorption", 8);
  mesh_ = createSceneMeshPtr(
      ponos::RawMeshes::cube(ponos::Transform(), false, true));
}

VolumeBox::VolumeBox(size_t w, size_t h, size_t d, float *data) : VolumeBox() {
  TextureAttributes ta;
  ta.width = w;
  ta.height = h;
  ta.depth = d;
  ta.internal_format = GL_RED;
  ta.format = GL_RED;
  ta.type = GL_FLOAT;
  ta.target = GL_TEXTURE_3D;
  TextureParameters tp;
  tp.target = GL_TEXTURE_3D;
  tp[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
  tp[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
  tp[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_BORDER;
  tp[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_BORDER;
  tp[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_BORDER;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  densityTexture.set(ta, tp);
  densityTexture.setTexels(data);
}

VolumeBox::~VolumeBox() = default;

void VolumeBox::draw(const CameraInterface *camera, ponos::Transform t) {
  UNUSED_VARIABLE(t);
  densityTexture.bind(GL_TEXTURE0);
  shader_->begin();
  shader_->setUniform("model_view_matrix",
                      ponos::transpose(camera->getViewTransform().matrix()));
  shader_->setUniform(
      "projection_matrix",
      ponos::transpose(camera->getProjectionTransform().matrix()));
  shader_->setUniform("cameraPosition", camera->getPosition());
  shader_->setUniform("g_densityTex", 0);
  shader_->setUniform("g_lightPos", lightPos);
  shader_->setUniform("g_lightIntensity", lightIntensity);
  shader_->setUniform("g_absorption", absorption);
  mesh_->bind();
  mesh_->vertexBuffer()->locateAttributes(*shader_.get());
  render(GL_BACK);
  mesh_->unbind();
  shader_->end();
}

const Texture &VolumeBox::texture() const { return densityTexture; }

Texture &VolumeBox::texture() { return densityTexture; }

void VolumeBox::update(float *data) {
  densityTexture.setTexels(reinterpret_cast<unsigned char *>(data));
}

void VolumeBox::render(GLenum cullFace) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(cullFace);
  glDrawElements(mesh_->indexBuffer()->bufferDescriptor.element_type,
                 mesh_->indexBuffer()->bufferDescriptor.element_count *
                     mesh_->indexBuffer()->bufferDescriptor.element_size,
                 GL_UNSIGNED_INT, nullptr);
}

} // namespace circe
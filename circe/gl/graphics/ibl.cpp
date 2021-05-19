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
///\file irradiance_map.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-05-16
///
///\brief

#include "ibl.h"
#include <circe/gl/io/framebuffer.h>
#include <circe/gl/scene/scene_model.h>
#include <circe/gl/graphics/shader.h>
#include <circe/scene/shapes.h>

namespace circe::gl {

void renderToCube(Framebuffer &framebuffer,
                  Program &program,
                  const Texture &envmap,
                  Texture &cubemap,
                  GLint mip_level,
                  const ponos::size2 &resolution) {
  SceneModel cube;
  cube = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}});

  auto projection = ponos::Transform::perspective(90, 1, 0.1, 10);
  ponos::Transform views[] = {
      ponos::Transform::lookAt({}, {1, 0, 0}, {0, -1, 0}),
      ponos::Transform::lookAt({}, {-1, 0, 0}, {0, -1, 0}),
      ponos::Transform::lookAt({}, {0, 1, 0}, {0, 0, -1}),
      ponos::Transform::lookAt({}, {0, -1, 0}, {0, 0, 1}),
      ponos::Transform::lookAt({}, {0, 0, -1}, {0, -1, 0}),
      ponos::Transform::lookAt({}, {0, 0, 1}, {0, -1, 0}),
  };

  framebuffer.resize(resolution);
  framebuffer.enable();
  glViewport(0, 0, resolution.width, resolution.height);
  envmap.bind(GL_TEXTURE0);
  program.use();
  program.setUniform("environmentMap", 0);
  program.setUniform("projection", projection);
  for (u32 i = 0; i < 6; ++i) {
    program.setUniform("view", views[i]);
    framebuffer.attachColorBuffer(cubemap.textureObjectId(),
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                  GL_COLOR_ATTACHMENT0,
                                  mip_level);
    framebuffer.enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.draw();
  }
  Framebuffer::disable();
}

Texture IBL::irradianceMap(const Texture &texture, const ponos::size2 &resolution) {
  Texture imap;

  std::string vs = "#version 330 core                                                                               \n"
                   "layout (location = 0) in vec3 aPos;                                                             \n"
                   "out vec3 localPos;                                                                              \n"
                   "uniform mat4 projection;                                                                        \n"
                   "uniform mat4 view;                                                                              \n"
                   "void main() {                                                                                   \n"
                   "    localPos = aPos;                                                                            \n"
                   "    gl_Position =  projection * view * vec4(localPos, 1.0);                                     \n"
                   "}";
  std::string fs = "#version 330 core                                                                               \n"
                   "out vec4 FragColor;                                                                             \n"
                   "in vec3 localPos;                                                                               \n"
                   "uniform samplerCube environmentMap;                                                             \n"
                   "const float PI = 3.14159265359;                                                                 \n"
                   "void main(){                                                                                    \n"
                   "    // the sample direction equals the hemisphere's orientation                                 \n"
                   "    vec3 N = normalize(localPos);                                                               \n"
                   "vec3 irradiance = vec3(0.0);                                                                    \n"
                   "vec3 up    = vec3(0.0, 1.0, 0.0);                                                               \n"
                   "vec3 right = normalize(cross(up, N));                                                           \n"
                   "up         = normalize(cross(N, right));                                                        \n"
                   "float sampleDelta = 0.025;                                                                      \n"
                   "float nrSamples = 0.0;                                                                          \n"
                   "for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {                                      \n"
                   "    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {                            \n"
                   "        // spherical to cartesian (in tangent space)                                            \n"
                   "        vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));   \n"
                   "        // tangent space to world                                                               \n"
                   "        vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;  \n"
                   "        irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);         \n"
                   "        nrSamples++;                                                                            \n"
                   "    }                                                                                           \n"
                   "}                                                                                               \n"
                   "irradiance = PI * irradiance * (1.0 / float(nrSamples));                                        \n"
                   "    FragColor = vec4(irradiance, 1.0);                                                          \n"
                   "}";
  Program program;
  program.attach(Shader(GL_VERTEX_SHADER, vs));
  program.attach(Shader(GL_FRAGMENT_SHADER, fs));
  if (!program.link())
    std::cerr << "failed to compile irradiance map shader!\n" << program.err << std::endl;
  // copy attributes
  imap.setTarget(texture.target());
  imap.setInternalFormat(texture.internalFormat());
  imap.setFormat(texture.format());
  imap.setType(texture.type());
  imap.resize(resolution);
  imap.bind();
  Texture::View(GL_TEXTURE_CUBE_MAP).apply();
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  Framebuffer frambuffer;
  frambuffer.setRenderBufferStorageInternalFormat(GL_DEPTH_COMPONENT24);

  renderToCube(frambuffer, program, texture, imap, 0, resolution);

  return imap;
}

Texture IBL::preFilteredEnvironmentMap(const Texture &input, const ponos::size2 &resolution) {
  ////////////////////////////////////// prepare texture //////////////////////////////////////////
  Texture prefilter_map;
  prefilter_map.setTarget(GL_TEXTURE_CUBE_MAP);
  prefilter_map.setInternalFormat(input.internalFormat());
  prefilter_map.setFormat(input.format());
  prefilter_map.setType(input.type());
  prefilter_map.resize(resolution);
  prefilter_map.bind();
  Texture::View view(GL_TEXTURE_CUBE_MAP);
  view[GL_TEXTURE_MIN_FILTER] = GL_LINEAR_MIPMAP_LINEAR;
  view.apply();
  prefilter_map.generateMipmap();
  prefilter_map.bind();
  ////////////////////////////////////// prepare shader //////////////////////////////////////////
  std::string vs = "#version 330 core                                                                               \n"
                   "layout (location = 0) in vec3 aPos;                                                             \n"
                   "out vec3 localPos;                                                                              \n"
                   "uniform mat4 projection;                                                                        \n"
                   "uniform mat4 view;                                                                              \n"
                   "void main() {                                                                                   \n"
                   "    localPos = aPos;                                                                            \n"
                   "    gl_Position =  projection * view * vec4(localPos, 1.0);                                     \n"
                   "}";
  std::string fs = "#version 330 core                                                                               \n"
                   "out vec4 FragColor;                                                                             \n"
                   "in vec3 localPos;                                                                               \n"
                   "uniform samplerCube environmentMap;                                                             \n"
                   "uniform float roughness;                                                                        \n"
                   "const float PI = 3.14159265359;                                                                 \n"
                   "float DistributionGGX(vec3 N, vec3 H, float roughness) {                                        \n"
                   "    float a = roughness*roughness;                                                              \n"
                   "    float a2 = a*a;                                                                             \n"
                   "    float NdotH = max(dot(N, H), 0.0);                                                          \n"
                   "    float NdotH2 = NdotH*NdotH;                                                                 \n"
                   "    float nom   = a2;                                                                           \n"
                   "    float denom = (NdotH2 * (a2 - 1.0) + 1.0);                                                  \n"
                   "    denom = PI * denom * denom;                                                                 \n"
                   "    return nom / denom;                                                                         \n"
                   "}                                                                                               \n"
                   "float RadicalInverse_VdC(uint bits) {                                                           \n"
                   "    bits = (bits << 16u) | (bits >> 16u);                                                       \n"
                   "    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);                         \n"
                   "    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);                         \n"
                   "    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);                         \n"
                   "    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);                         \n"
                   "    return float(bits) * 2.3283064365386963e-10; // / 0x100000000                               \n"
                   "}                                                                                               \n"
                   "vec2 Hammersley(uint i, uint N) {                                                               \n"
                   "  return vec2(float(i)/float(N), RadicalInverse_VdC(i));                                        \n"
                   "}                                                                                               \n"
                   "vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {                                    \n"
                   "    float a = roughness*roughness;                                                              \n"
                   "    float phi = 2.0 * PI * Xi.x;                                                                \n"
                   "    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));                           \n"
                   "    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);                                             \n"
                   "    // from spherical coordinates to cartesian coordinates                                      \n"
                   "    vec3 H;                                                                                     \n"
                   "    H.x = cos(phi) * sinTheta;                                                                  \n"
                   "    H.y = sin(phi) * sinTheta;                                                                  \n"
                   "    H.z = cosTheta;                                                                             \n"
                   "    // from tangent-space vector to world-space sample vector                                   \n"
                   "    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);              \n"
                   "    vec3 tangent   = normalize(cross(up, N));                                                   \n"
                   "    vec3 bitangent = cross(N, tangent);                                                         \n"
                   "    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;                                 \n"
                   "    return normalize(sampleVec);                                                                \n"
                   "}                                                                                               \n"
                   "void main() {                                                                                   \n"
                   "    vec3 N = normalize(localPos);                                                               \n"
                   "    vec3 R = N;                                                                                 \n"
                   "    vec3 V = R;                                                                                 \n"
                   "    const uint SAMPLE_COUNT = 1024u;                                                            \n"
                   "    float totalWeight = 0.0;                                                                    \n"
                   "    vec3 prefilteredColor = vec3(0.0);                                                          \n"
                   "    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {                                                   \n"
                   "        vec2 Xi = Hammersley(i, SAMPLE_COUNT);                                                  \n"
                   "        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);                                        \n"
                   "        vec3 L  = normalize(2.0 * dot(V, H) * H - V);                                           \n"
                   "        float NdotL = max(dot(N, L), 0.0);                                                      \n"
                   "        if(NdotL > 0.0) {                                                                       \n"
                   "            // sample from the environment's mip level based on roughness/pdf                   \n"
                   "            float D   = DistributionGGX(N, H, roughness);                                       \n"
                   "            float NdotH = max(dot(N, H), 0.0);                                                  \n"
                   "            float HdotV = max(dot(H, V), 0.0);                                                  \n"
                   "            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;                                     \n"
                   "            float resolution = 512.0; // resolution of source cubemap (per face)                \n"
                   "            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);                        \n"
                   "            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);                        \n"
                   "            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);           \n"
                   "            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;            \n"
                   "            totalWeight      += NdotL;                                                          \n"
                   "        }                                                                                       \n"
                   "    }                                                                                           \n"
                   "    prefilteredColor = prefilteredColor / totalWeight;                                          \n"
                   "    FragColor = vec4(prefilteredColor, 1.0);                                                    \n"
                   "}";
  Program program;
  program.attach(Shader(GL_VERTEX_SHADER, vs));
  program.attach(Shader(GL_FRAGMENT_SHADER, fs));
  if (!program.link())
    std::cerr << "failed to compile IBL::prefilter map shader!\n" << program.err << std::endl;
  ////////////////////////////////////// prepare shader //////////////////////////////////////////
  Framebuffer framebuffer;
  framebuffer.setRenderBufferStorageInternalFormat(GL_DEPTH_COMPONENT24);
  ////////////////////////////////////// filter  /////////////////////////////////////////////////
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  unsigned int max_mip_levels = 5;
  for (unsigned int mip = 0; mip < max_mip_levels; ++mip) {
    u32 mip_width = 128 * std::pow(0.5, mip);
    u32 mip_height = 128 * std::pow(0.5, mip);
    f32 roughness = (float) mip / (float) (max_mip_levels - 1);
    program.use();
    program.setUniform("roughness", roughness);

    renderToCube(framebuffer, program, input, prefilter_map, mip, {mip_width, mip_height});
  }
  return prefilter_map;
}

Texture IBL::brdfIntegrationMap(const ponos::size2 &resolution) {
  ////////////////////////////////////// prepare shader //////////////////////////////////////////
  std::string vs = "#version 330 core                                                                               \n"
                   "layout (location = 0) in vec3 aPos;                                                             \n"
                   "layout (location = 1) in vec2 aTexCoords;                                                       \n"
                   "out vec2 TexCoords;                                                                             \n"
                   "void main() {                                                                                   \n"
                   "    TexCoords = aTexCoords;                                                                     \n"
                   "    gl_Position = vec4(aPos, 1.0);                                                              \n"
                   "}";
  std::string fs = "#version 330 core                                                                               \n"
                   "out vec2 FragColor;                                                                             \n"
                   "in vec2 TexCoords;                                                                              \n"
                   "const float PI = 3.14159265359;                                                                 \n"
                   "float RadicalInverse_VdC(uint bits) {                                                           \n"
                   "    bits = (bits << 16u) | (bits >> 16u);                                                       \n"
                   "    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);                         \n"
                   "    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);                         \n"
                   "    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);                         \n"
                   "    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);                         \n"
                   "    return float(bits) * 2.3283064365386963e-10; // / 0x100000000                               \n"
                   "}                                                                                               \n"
                   "vec2 Hammersley(uint i, uint N) {                                                               \n"
                   "  return vec2(float(i)/float(N), RadicalInverse_VdC(i));                                        \n"
                   "}                                                                                               \n"
                   "vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {                                    \n"
                   "    float a = roughness*roughness;                                                              \n"
                   "    float phi = 2.0 * PI * Xi.x;                                                                \n"
                   "    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));                           \n"
                   "    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);                                             \n"
                   "    // from spherical coordinates to cartesian coordinates                                      \n"
                   "    vec3 H;                                                                                     \n"
                   "    H.x = cos(phi) * sinTheta;                                                                  \n"
                   "    H.y = sin(phi) * sinTheta;                                                                  \n"
                   "    H.z = cosTheta;                                                                             \n"
                   "    // from tangent-space vector to world-space sample vector                                   \n"
                   "    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);              \n"
                   "    vec3 tangent   = normalize(cross(up, N));                                                   \n"
                   "    vec3 bitangent = cross(N, tangent);                                                         \n"
                   "    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;                                 \n"
                   "    return normalize(sampleVec);                                                                \n"
                   "}                                                                                               \n"
                   "float GeometrySchlickGGX(float NdotV, float roughness) {                                        \n"
                   "    // note that we use a different k for IBL                                                   \n"
                   "    float a = roughness;                                                                        \n"
                   "    float k = (a * a) / 2.0;                                                                    \n"
                   "    float nom   = NdotV;                                                                        \n"
                   "    float denom = NdotV * (1.0 - k) + k;                                                        \n"
                   "    return nom / denom;                                                                         \n"
                   "}                                                                                               \n"
                   "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {                                  \n"
                   "    float NdotV = max(dot(N, V), 0.0);                                                          \n"
                   "    float NdotL = max(dot(N, L), 0.0);                                                          \n"
                   "    float ggx2 = GeometrySchlickGGX(NdotV, roughness);                                          \n"
                   "    float ggx1 = GeometrySchlickGGX(NdotL, roughness);                                          \n"
                   "    return ggx1 * ggx2;                                                                         \n"
                   "}                                                                                               \n"
                   "vec2 IntegrateBRDF(float NdotV, float roughness) {                                              \n"
                   "    vec3 V;                                                                                     \n"
                   "    V.x = sqrt(1.0 - NdotV*NdotV);                                                              \n"
                   "    V.y = 0.0;                                                                                  \n"
                   "    V.z = NdotV;                                                                                \n"
                   "    float A = 0.0;                                                                              \n"
                   "    float B = 0.0;                                                                              \n"
                   "    vec3 N = vec3(0.0, 0.0, 1.0);                                                               \n"
                   "    const uint SAMPLE_COUNT = 1024u;                                                            \n"
                   "    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {                                                   \n"
                   "        // generates a sample vector that's biased towards the                                  \n"
                   "        // preferred alignment direction (importance sampling).                                 \n"
                   "        vec2 Xi = Hammersley(i, SAMPLE_COUNT);                                                  \n"
                   "        vec3 H = ImportanceSampleGGX(Xi, N, roughness);                                         \n"
                   "        vec3 L = normalize(2.0 * dot(V, H) * H - V);                                            \n"
                   "        float NdotL = max(L.z, 0.0);                                                            \n"
                   "        float NdotH = max(H.z, 0.0);                                                            \n"
                   "        float VdotH = max(dot(V, H), 0.0);                                                      \n"
                   "        if(NdotL > 0.0) {                                                                       \n"
                   "            float G = GeometrySmith(N, V, L, roughness);                                        \n"
                   "            float G_Vis = (G * VdotH) / (NdotH * NdotV);                                        \n"
                   "            float Fc = pow(1.0 - VdotH, 5.0);                                                   \n"
                   "            A += (1.0 - Fc) * G_Vis;                                                            \n"
                   "            B += Fc * G_Vis;                                                                    \n"
                   "        }                                                                                       \n"
                   "    }                                                                                           \n"
                   "    A /= float(SAMPLE_COUNT);                                                                   \n"
                   "    B /= float(SAMPLE_COUNT);                                                                   \n"
                   "    return vec2(A, B);                                                                          \n"
                   "}                                                                                               \n"
                   "void main() {                                                                                   \n"
                   "    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);                              \n"
                   "    FragColor = integratedBRDF;                                                                 \n"
                   "}";
  Program program;
  program.attach(Shader(GL_VERTEX_SHADER, vs));
  program.attach(Shader(GL_FRAGMENT_SHADER, fs));
  if (!program.link())
    std::cerr << "failed to compile IBL::brdfIntegration map shader!\n" << program.err << std::endl;
  ////////////////////////////////////// prepare texture //////////////////////////////////////////
  Texture brdf_i_map;
  brdf_i_map.setTarget(GL_TEXTURE_2D);
  brdf_i_map.setInternalFormat(GL_RG16F);
  brdf_i_map.setFormat(GL_RG);
  brdf_i_map.setType(GL_FLOAT);
  brdf_i_map.resize(resolution);
  brdf_i_map.bind();
  Texture::View().apply();
  ////////////////////////////////////// generate texture /////////////////////////////////////////
  SceneModel quad;
  quad = Shapes::box({{-1, -1}, {1, 1}}, shape_options::uv);

  program.use();

  Framebuffer framebuffer;
  framebuffer.setRenderBufferStorageInternalFormat(GL_DEPTH_COMPONENT24);
  framebuffer.resize(resolution);
  glViewport(0, 0, resolution.width, resolution.height);
  framebuffer.attachTexture(brdf_i_map);
  framebuffer.enable();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  quad.draw();

  return brdf_i_map;
}

}

// Created by filipecn on 3/2/18.
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
#include "post_effect.h"

namespace circe::gl {

PostEffect::PostEffect(ShaderProgram *s) {
  if (!s)
    shader.reset(new ShaderProgram(ShaderManager::instance().loadFromTexts(
        CIRCE_NO_VAO_VS, nullptr, CIRCE_NO_VAO_FS)));
  else
    shader.reset(s);
}

void PostEffect::apply(const RenderTexture &in, RenderTexture &out) {
  in.bind(GL_TEXTURE0);
  shader->setUniform("tex", 0);
  out.render([&]() {
    shader->begin();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shader->end();
  });
}

FXAA::FXAA() {
  const char *vs = "#version 440 core\n"
                   "out vec2 v_rgbNW;"
                   "out vec2 v_rgbNE;"
                   "out vec2 v_rgbSW;"
                   "out vec2 v_rgbSE;"
                   "out vec2 v_rgbM;"
                   "out vec2 texCoord;"
                   "uniform vec2 texSize;"
                   "void main(void) {"
                   "    float x = -1.0 + float((gl_VertexID & 1) << 2);"
                   "    float y = -1.0 + float((gl_VertexID & 2) << 1);"
                   "    texCoord.x = (x+1.0)*0.5;"
                   "    texCoord.y = (y+1.0)*0.5;"
                   "    vec2 fragCoord = texCoord * texSize;"
                   "    vec2 inverseVP = 1.0 / texSize.xy;\n"
                   "    v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;\n"
                   "    v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;\n"
                   "    v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;\n"
                   "    v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;\n"
                   "    v_rgbM = vec2(fragCoord * inverseVP);"
                   "    gl_Position = vec4(x, y, 0, 1);"
                   "}";
  const char *fs =
      "#version 440 core\n"
      "#define FXAA_REDUCE_MIN   (1.0/ 128.0)\n"
      "#define FXAA_REDUCE_MUL   (1.0 / 8.0)\n"
      "#define FXAA_SPAN_MAX     8.0\n"
      "in vec2 v_rgbNW;"
      "in vec2 v_rgbNE;"
      "in vec2 v_rgbSW;"
      "in vec2 v_rgbSE;"
      "in vec2 v_rgbM;"
      "in vec2 texCoord;"
      "out vec4 outColor;"
      "uniform vec2 texSize;"
      "uniform sampler2D tex;"
      "void main(void) {"
      "    vec2 fragCoord = texCoord * texSize;"
      "    vec2 inverseVP = vec2(1.0 / texSize.x, 1.0 / texSize.y);\n"
      "    vec3 rgbNW = texture2D(tex, v_rgbNW).xyz;\n"
      "    vec3 rgbNE = texture2D(tex, v_rgbNE).xyz;\n"
      "    vec3 rgbSW = texture2D(tex, v_rgbSW).xyz;\n"
      "    vec3 rgbSE = texture2D(tex, v_rgbSE).xyz;\n"
      "    vec4 texColor = texture2D(tex, v_rgbM);\n"
      "    vec3 rgbM  = texColor.xyz;\n"
      "    vec3 luma = vec3(0.299, 0.587, 0.114);\n"
      "    float lumaNW = dot(rgbNW, luma);\n"
      "    float lumaNE = dot(rgbNE, luma);\n"
      "    float lumaSW = dot(rgbSW, luma);\n"
      "    float lumaSE = dot(rgbSE, luma);\n"
      "    float lumaM  = dot(rgbM,  luma);\n"
      "    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, "
      "lumaSE)));\n"
      "    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, "
      "lumaSE)));"
      "    vec2 dir;\n"
      "    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));\n"
      "    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));\n"
      "    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *\n"
      "                          (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);\n"
      "    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);\n"
      "    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),\n"
      "              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),\n"
      "              dir * rcpDirMin)) * inverseVP;\n"
      "    vec3 rgbA = 0.5 * (\n"
      "        texture2D(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - "
      "0.5)).xyz +\n"
      "        texture2D(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - "
      "0.5)).xyz);\n"
      "    vec3 rgbB = rgbA * 0.5 + 0.25 * (\n"
      "        texture2D(tex, fragCoord * inverseVP + dir * -0.5).xyz +\n"
      "        texture2D(tex, fragCoord * inverseVP + dir * 0.5).xyz);"
      "    float lumaB = dot(rgbB, luma);\n"
      "    if ((lumaB < lumaMin) || (lumaB > lumaMax))\n"
      "        outColor = vec4(rgbA, texColor.a);\n"
      "    else\n"
      "        outColor = vec4(rgbB, texColor.a);\n"
      "}";

  shader.reset(new ShaderProgram(
      ShaderManager::instance().loadFromTexts(vs, nullptr, fs)));
}

void FXAA::apply(const RenderTexture &in, RenderTexture &out) {
  in.bind(GL_TEXTURE0);
  shader->setUniform("tex", 0);
  shader->setUniform("texSize", ponos::vec2(in.size()[0], in.size()[1]));
  out.render([&]() {
    shader->begin();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shader->end();
  });
}

GammaCorrection::GammaCorrection(float g) : gamma(g) {
  const char *fs = "#version 440 core\n"
                   "out vec4 outColor;"
                   "in vec2 texCoord;"
                   "uniform sampler2D tex;"
                   "uniform float gammaBound;"
                   "void main() {"
                   "outColor = texture(tex, texCoord);"
                   "outColor.rgb = pow(outColor.rgb, vec3(1.0 / gammaBound));"
                   "}";

  shader.reset(new ShaderProgram(
      ShaderManager::instance().loadFromTexts(CIRCE_NO_VAO_VS, nullptr, fs)));
  shader->addUniform("tex", 0);
  shader->addUniform("gammaBound", 1);
}

void GammaCorrection::apply(const RenderTexture &in, RenderTexture &out) {
  in.bind(GL_TEXTURE0);
  shader->setUniform("tex", 0);
  shader->setUniform("gammaBound", gamma);
  out.render([&]() {
    shader->begin();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shader->end();
  });
}

} // circe namespace

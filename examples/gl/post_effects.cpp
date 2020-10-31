// Created by filipecn on 3/3/18.
#include <circe/circe.h>

#define WIDTH 800
#define HEIGHT 800

const char *fs = "#version 440 core\n"
                 "out vec4 outColor;"
                 "in vec2 texCoord;"
                 "uniform sampler2D tex;"
                 "void main() {"
                 "ivec2 texel = ivec2(gl_FragCoord.xy);"
                 "outColor = (texelFetchOffset(tex, texel, 0, ivec2(0, 0)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(1, 1)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(0, 1)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(-1, 1)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(-1, 0)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(1, 0)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(-1, -1)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(1, -1)) +"
                 "texelFetchOffset(tex, texel, 0, ivec2(0, -1))) / 9.0;"
                 "}";

int main() {
  circe::gl::SceneApp<> app(WIDTH, HEIGHT, "Post Effects Example");
  app.init();
  app.viewports[0].renderer->addEffect(new circe::gl::GammaCorrection());
  //  app.viewports[0].renderer->addEffect(new circe::FXAA());
  //  app.viewports[0].renderer->addEffect(new circe::PostEffect(
  //      new circe::Shader(circe_NO_VAO_VS, nullptr, fs)));
  app.scene.add(new circe::gl::CartesianGrid(5));
  app.run();
  return 0;
}
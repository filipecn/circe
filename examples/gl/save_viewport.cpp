// Created by filipecn on 9/3/18.
#include <circe/circe.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace circe::gl;

int main() {
  SceneApp<> app(800, 800);
  app.scene.add(new CartesianGrid(5));
  app.buttonCallback = [&](int button, int action, int modifiers) {
    if (action == GLFW_RELEASE) {
      std::vector<unsigned char> data;
      size_t w = 0, h = 0;
      app.viewports[0].renderer->currentPixels(data, w, h);
      stbi_write_png("viewport.png", w, h, 4, data.data(), w * 4);
    }
  };
  return app.run();
}

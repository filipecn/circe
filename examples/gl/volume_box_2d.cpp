#include <circe/circe.h>

int main() {
  // volume
  size_t res = 128;
  std::vector<float> volumeData(res * res);
  for (size_t y = 0; y < res; y++)
    for (size_t x = 0; x < res; x++)
      if ((ponos::vec2(x, y) - ponos::vec2(res / 2.f)).length() <= 20)
        volumeData[y * res + x] = 1.f;

  // app
  circe::gl::SceneApp<> app(800, 800, "Volume Box Example", false);
  app.addViewport2D(0,0,800,800);
  circe::gl::CartesianGrid grid(5);
  app.scene.add(&grid);
  circe::gl::VolumeBox2 vb(res, res, volumeData.data());
  app.scene.add(&vb);
  app.run();
  return 0;
}

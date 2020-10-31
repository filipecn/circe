#include <circe/circe.h>

int main() {
  // volume
  size_t res = 128;
  std::vector<float> volumeData(res * res * res);
  for (size_t z = 0; z < res; z++)
    for (size_t y = 0; y < res; y++)
      for (size_t x = 0; x < res; x++)
        if ((ponos::vec3(x, y, z) - ponos::vec3(res / 2.f)).length() <= 20)
          volumeData[z * res * res + y * res + x] = 1.f;

  // app
  circe::gl::SceneApp<> app(800, 800, "Volume Box Example");
  circe::gl::CartesianGrid grid(5);
  app.scene.add(&grid);
  circe::gl::VolumeBox vb(res, res, res, volumeData.data());
  app.scene.add(&vb);
  app.run();
  return 0;
}
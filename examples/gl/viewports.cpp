#include <circe/circe.h>

#define WIDTH 800
#define HEIGHT 800

int main() {
  circe::gl::SceneApp<> app(WIDTH, HEIGHT, "Viewports Example", false);
  // app.addViewport2D(0 , 0, WIDTH, HEIGHT);
  // app.getCamera<circe::UserCamera2D>(0)->setPosition(ponos::Point3(0.5,0,0));
  app.addViewport2D(0, 0, WIDTH / 2, HEIGHT / 2);
  app.getCamera<circe::UserCamera2D>(0)->setPosition(ponos::point3(1, 0, 0));
  //  app.getCamera<circe::UserCamera2D>(0)->fit(ponos::BBox2D::unitBox());
  app.addViewport(WIDTH / 2, 0, WIDTH / 2, HEIGHT / 2);
  app.addViewport2D(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
  app.getCamera<circe::UserCamera2D>(2)->setPosition(ponos::point3(0, 1, 0));
  app.addViewport2D(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
  app.scene.add(new circe::gl::CartesianGrid(5));
  return app.run();
}
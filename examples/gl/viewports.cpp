#include <circe/circe.h>

#define WIDTH 400
#define HEIGHT 400

struct ViewportsExample : public circe::gl::BaseApp {
  ViewportsExample() : circe::gl::BaseApp(2 * WIDTH, 2 * HEIGHT, "Viewports Example", false) {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // setup viewports
    //                |
    //    cameras     |     PERSPECTIVE
    //                |
    // -----------------------------------
    //                |
    // z    LEFT      | y      TOP
    //   x            |   z

    // top view viewport (XZ plane)
    this->app->addViewport2D(0, 0, WIDTH, HEIGHT);
    this->app->getCamera<circe::UserCamera2D>(0)->setPosition({1, 0, 0});
    // left view viewport (YZ plane)
    this->app->addViewport2D(WIDTH, 0, WIDTH, HEIGHT);
    this->app->getCamera<circe::UserCamera2D>(1)->setPosition({0, 1, 0});
    this->app->addViewport(0, HEIGHT, WIDTH, HEIGHT);
    // perspective view viewport
    this->app->addViewport(WIDTH, HEIGHT, WIDTH, HEIGHT);

    // scene
    model = circe::Shapes::box(hermes::bbox3::unitBox(true), circe::shape_options::uv);
    HERMES_ASSERT_WITH_LOG(model.program.link(shaders_path, "color"), model.program.err)
  }

  void prepareFrame() override {
    BaseApp::prepareFrame();
    top.update(app->getCamera(0));
    front.update(app->getCamera(1));
    perspective.update(app->getCamera(2));
  }

  void render(circe::CameraInterface *camera) override {
    static size_t current_viewport = 0;

    model.program.use();
    model.program.setUniform("projection", camera->getProjectionTransform());
    model.program.setUniform("model", camera->getModelTransform());
    model.program.setUniform("view", camera->getViewTransform());
    model.program.setUniform("color", circe::Color::Black());

    model.draw();

    if(current_viewport == 2) {
      front.draw(camera);
      top.draw(camera);
      perspective.draw(camera);
    }

    cartesian_grid.draw(camera);

    current_viewport = (current_viewport + 1) % 4;
  }

  circe::gl::SceneModel model;
  circe::gl::helpers::CameraModel top;
  circe::gl::helpers::CameraModel front;
  circe::gl::helpers::CameraModel perspective;
  circe::gl::helpers::CartesianGrid cartesian_grid;
};

int main() {
  return ViewportsExample().run();
}
#include <circe/circe.h>

#define WIDTH 400
#define HEIGHT 400

struct ViewportsExample : public circe::gl::BaseApp {
  ViewportsExample() : circe::gl::BaseApp(2 * WIDTH, 2 * HEIGHT, "Viewports Example", circe::app_options::no_viewport) {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // setup viewports_
    //                |
    //    cameras     |     PERSPECTIVE
    //                |
    // -----------------------------------
    //                |
    // z    LEFT      | y      TOP
    //   x            |   z

    // top view viewport (XZ plane)
    this->app->addViewport2D(0, 0, WIDTH, HEIGHT);
    this->app->viewport(0).camera().setPosition({1, 0, 0});
    // left view viewport (YZ plane)
    this->app->addViewport2D(WIDTH, 0, WIDTH, HEIGHT);
    this->app->viewport(1).camera().setPosition({0, 1, 0});
    this->app->addViewport(0, HEIGHT, WIDTH, HEIGHT);
    // perspective view viewport
    this->app->addViewport(WIDTH, HEIGHT, WIDTH, HEIGHT);

    // scene
    model = circe::Shapes::box(hermes::bbox3::unitBox(true), circe::shape_options::uv);
    HERMES_ASSERT_WITH_LOG(model.program.link(shaders_path, "color"), model.program.err)
  }

  void prepareFrame() override {
    BaseApp::prepareFrame();
    top.update(&app->viewport(0).camera());
    front.update(&app->viewport(1).camera());
    perspective.update(&app->viewport(2).camera());
  }

  void render(circe::CameraInterface *camera) override {
    static size_t current_viewport = 0;

    model.program.use();
    model.program.setUniform("projection", camera->getProjectionTransform());
    model.program.setUniform("model", camera->getModelTransform());
    model.program.setUniform("view", camera->getViewTransform());
    model.program.setUniform("color", circe::Color::Black());

    model.draw();

    if (current_viewport == 2) {
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
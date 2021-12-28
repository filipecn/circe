#include <circe/circe.h>

#define WIDTH 800
#define HEIGHT 800

struct ViewportsExample : public circe::gl::BaseApp {
  ViewportsExample() : circe::gl::BaseApp(2 * WIDTH, 2 * HEIGHT, "Viewports Example", false) {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // setup viewports
    //                |
    // z     TOP      | y      LEFT
    //   x            |   z
    // -----------------------------------
    //                |
    // y    FRONT     |     PERSPECTIVE
    //   x            |

    // top view viewport (XZ plane)
    this->app_->addViewport2D(0, 0, WIDTH, HEIGHT);
    this->app_->getCamera<circe::UserCamera2D>(0)->setPosition({0, 1, 0});
    // left view viewport (YZ plane)
    this->app_->addViewport2D(WIDTH, 0, WIDTH, HEIGHT);
    this->app_->getCamera<circe::UserCamera2D>(1)->setPosition({1, 0, 0});
    // front view viewport (YX plane)
    this->app_->addViewport2D(0, HEIGHT, WIDTH, HEIGHT);
    this->app_->getCamera<circe::UserCamera2D>(2)->setPosition({0, 0, 1});
    // perspective view viewport
    this->app_->addViewport(WIDTH, HEIGHT, WIDTH, HEIGHT);

    this->app_->viewports[0].clear_screen_color = circe::Color::White();
    this->app_->viewports[1].clear_screen_color = circe::Color::Red();
    this->app_->viewports[2].clear_screen_color = circe::Color::Green();
    this->app_->viewports[3].clear_screen_color = circe::Color::Blue();

    // scene
    model = circe::Shapes::box(hermes::bbox3::unitBox(), circe::shape_options::uv);
    HERMES_ASSERT_WITH_LOG(model.program.link(shaders_path, "color"), model.program.err)
  }

  void render(circe::CameraInterface *camera) override {
    model.program.use();
    model.program.setUniform("projection", camera->getProjectionTransform());
    model.program.setUniform("model", camera->getModelTransform());
    model.program.setUniform("view", camera->getViewTransform());
    model.program.setUniform("color", circe::Color::Black());

    model.draw();
  }

  circe::gl::SceneModel model;
};

int main() {
  return ViewportsExample().run();
}
#include <circe/circe.h>

struct alignas(16) vec3_16 {
  vec3_16 &operator=(const circe::Color &color) {
    x = color.r;
    y = color.g;
    z = color.b;
    return *this;
  }
  float x{0};
  float y{0};
  float z{0};
};

struct Light {
  vec3_16 position;
  vec3_16 ambient;
  vec3_16 diffuse;
  vec3_16 specular;
};

class DepthBufferView {
public:
  DepthBufferView() {
    // setup texture image
    circe::gl::TextureAttributes attributes;
    attributes.target = GL_TEXTURE_2D;
    attributes.width = 256;
    attributes.height = 256;
    attributes.type = GL_UNSIGNED_BYTE;
    attributes.internal_format = GL_RGBA8;
    attributes.format = GL_RGBA;
    circe::gl::TextureParameters parameters;
    image.set(attributes, parameters);
    // setup shader
    ponos::Path shaders_path(std::string(SHADERS_PATH));
    if (!program.link({shaders_path + "depth_buffer.vert", shaders_path + "depth_buffer.frag"}))
      spdlog::error(program.err);
    // setup screen quad
    std::vector<f32> vertices = {
        // x y u v
        -1.0, -1.0, 0.0, 0.0, // 0
        1.0, -1.0, 1.0, 0.0, // 1
        1.0, 1.0, 1.0, 1.0, // 2
        -1.0, 1.0, 0.0, 1.0, // 3
    };
    std::vector<u32> indices = {
        0, 1, 2, // 0
        0, 2, 3, // 1
    };
    vb.attributes.push<ponos::point2>("position");
    vb.attributes.push<ponos::vec2>("uv");
    vb = vertices;
    ib.element_count = 2;
    ib = indices;
    vao.bind();
    vb.bindAttributeFormats();
  }
  void update() {
    image.render([&]() {
      vao.bind();
      vb.bind();
      ib.bind();
      program.use();
      program.setUniform("depthMap", 0);
      ib.draw();
    });
  }
  circe::gl::Program program;
  circe::gl::VertexArrayObject vao;
  circe::gl::VertexBuffer vb;
  circe::gl::IndexBuffer ib;
  circe::gl::RenderTexture image;
};

class HelloCirce : public circe::gl::BaseApp {
public:
  struct SceneUniformBufferData {
    Light light;
  } scene_ubo_data;

  HelloCirce() : BaseApp(800, 800) {
    // setup object model
    ponos::Path assets_path(std::string(ASSETS_PATH));
    // setup reference grid
    ponos::Path shaders_path(std::string(SHADERS_PATH));
    // init shadow map
    circe::Light l;
    l.type = circe::LightTypes::DIRECTIONAL;
    l.direction = ponos::normalize(ponos::vec3(-1.0f, 1.0f, 1.f));
    shadow_map.setLight(l);
    // setup scene uniform buffer
    scene_ubo_data.light.position.x = l.direction.x * 20;
    scene_ubo_data.light.position.y = l.direction.y * 20;
    scene_ubo_data.light.position.z = l.direction.z * 20;
    scene_ubo_data.light.diffuse = scene_ubo_data.light.specular = scene_ubo_data.light.ambient =
        circe::Color(1.0, 0.5, 0.5);
//    scene_ubo.push(object.program);
//    object.program.setUniformBlockBinding("Scene", 0);
//    scene_ubo["Scene"] = &scene_ubo_data;

    normal_map = circe::gl::Texture::fromFile(assets_path + "brickwall.jpg");
    /// load cartesian plane
    cartesian_plane = circe::Shapes::plane(ponos::Plane::XY(),
                                           ponos::point3(),
                                           ponos::vec3(100, 0, 0),
                                           20,
                                           circe::shape_options::tangent_space | circe::shape_options::normal);
    if (!cartesian_plane.program.link(shaders_path, "scene_object"))
      spdlog::error("Failed to load model shader");
    /// load mesh
    mesh = circe::gl::SceneModel::fromFile(assets_path + "suzanne.obj");
    if (!mesh.program.link(shaders_path, "scene_object"))
      spdlog::error("Failed to load model shader");
  }

  void prepareFrame(const circe::gl::ViewportDisplay &display) override {
    circe::gl::BaseApp::prepareFrame(display);
    auto p = app_->getCamera()->getPosition();
    circe::Light l;
    l.type = circe::LightTypes::DIRECTIONAL;
    l.direction = ponos::normalize(ponos::vec3(p.x, p.y, p.z));
    shadow_map.setLight(l);
    shadow_map.render([&]() {
//      glDisable( GL_CULL_FACE );
      cartesian_plane.draw();
      mesh.draw();
//      glEnable( GL_CULL_FACE );
    });
  }

  void render(circe::CameraInterface *camera) override {
    // render reference grid
    cartesian_plane.program.use();
    cartesian_plane.program.setUniform("view",
                                       ponos::transpose(camera->getViewTransform().matrix()));
    cartesian_plane.program.setUniform("projection",
                                       ponos::transpose(camera->getProjectionTransform().matrix()));
    shadow_map.depthMap().bind(GL_TEXTURE0);
    cartesian_plane.draw();
    // render model
    mesh.program.use();
    mesh.program.setUniform("view",
                            ponos::transpose(camera->getViewTransform().matrix()));
    mesh.program.setUniform("projection",
                            ponos::transpose(camera->getProjectionTransform().matrix()));
//    mesh.program.setUniform("material.kAmbient", circe::Color::White());
//    mesh.program.setUniform("material.kDiffuse", circe::Color::White());
//    mesh.program.setUniform("material.kSpecular", circe::Color::White());
//    mesh.program.setUniform("material.shininess", 32);
//    mesh.program.setUniform("model", ponos::transpose(ponos::Transform().matrix()));
//    mesh.program.setUniform("lightSpaceMatrix", ponos::transpose(shadow_map.light_transform().matrix()));
//    mesh.program.setUniform("shadowMap", 0);
//    mesh.program.setUniform("cameraPosition", camera->getPosition());
    mesh.draw();

    shadow_map.depthMap().bind(GL_TEXTURE0);
    depth_buffer_view.update();
    ImGui::Begin("Shadow Map");
    texture_id = shadow_map.depthMap().textureObjectId();
    texture_id = depth_buffer_view.image.textureObjectId();
//    normal_map.bind(GL_TEXTURE0);
//    texture_id = normal_map.textureObjectId();
    ImGui::Image((void *) (intptr_t) (texture_id), {256, 256},
                 {0, 1}, {1, 0});
    ImGui::End();
  }

  circe::gl::Texture normal_map;
  DepthBufferView depth_buffer_view;
  circe::gl::ShadowMap shadow_map;
  circe::gl::UniformBuffer scene_ubo;
  circe::gl::SceneModel mesh, cartesian_plane;
  GLuint texture_id{0};
};

int main() {
  return HelloCirce().run();
}

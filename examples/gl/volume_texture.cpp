/// Copyright (c) 2021, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file volume_texture.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-21
///
///\brief

#include <circe/circe.h>

hermes::vec2 rand_state;

float rand(float n) { return hermes::Numbers::fract(sin(n) * 43758.5453123); }

float rand(hermes::vec2 n) {
  return hermes::Numbers::fract(sin(dot(n, hermes::vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(float p) {
  float fl = floor(p);
  float fc = hermes::Numbers::fract(p);
  return hermes::interpolation::mix(rand(fl), rand(fl + 1.0), fc);
}

float noise(hermes::vec2 n) {
  const hermes::vec2 d = hermes::vec2(0.0, 1.0);
  hermes::vec2 b = floor(n), f = hermes::interpolation::smoothStep(0.0, 1.0,
                                                                   hermes::vec2(
                                                                       hermes::Numbers::fract(n.x),
                                                                       hermes::Numbers::fract(n.y)
                                                                   ));
  return hermes::Numbers::fract(std::abs(hermes::interpolation::mix(hermes::interpolation::mix(rand(b),
                                                                                               rand(b + d.yx()),
                                                                                               f.x),
                                                                    hermes::interpolation::mix(rand(b + d.xy()),
                                                                                               rand(b + d.yy()),
                                                                                               f.x),
                                                                    f.y)));
}

float noise() {
  rand_state = rand_state * hermes::vec2(1.4235233, 1.112325);
  auto n = noise(rand_state);
  return n;
}

float getSpiral(const hermes::point3 &p, float out_value) {
  const float r = 0.5f * (0.5f - fabsf(p.y));
  const float a = (float) (hermes::Constants::pi * 8.0) * p.y;
  const float dx = (cosf(a) * r - p.x) * 2.0f;
  const float dy = (sinf(a) * r - p.z) * 2.0f;
  return powf(fmaxf((1.0f - dx * dx - dy * dy), 0.0f), 8.0f) *
      out_value;
}

float getMenger(const hermes::point3 &p, float out_value) {
  hermes::point3 pos = p + hermes::vec3(0.5f, 0.5f, 0.5f);
  const unsigned int steps = 3;
  for (unsigned int i = 0; i < steps; ++i) {
    pos *= 3.0f;
    const int s = ((int) pos.x & 1) + ((int) pos.y & 1) + ((int) pos.z & 1);
    if (s >= 2)
      return 0.0f;
  }
  return out_value;
}

bool inVolume(hermes::point3 pos) {
  static hermes::point3 domain_box_lower(-0.5);
  static hermes::point3 domain_box_upper(0.5);
  return pos.x > domain_box_lower.x && pos.y > domain_box_lower.y &&
      pos.z > domain_box_lower.z && pos.x < domain_box_upper.x &&
      pos.y < domain_box_upper.y && pos.z < domain_box_upper.z;
}

bool intersectVolumeBox(f32 &tmin, hermes::point3 raypos, hermes::vec3 raydir) {
  const float x0 = (-0.5f - raypos.x) / raydir.x;
  const float y0 = (-0.5f - raypos.y) / raydir.y;
  const float z0 = (-0.5f - raypos.z) / raydir.z;
  const float x1 = (0.5f - raypos.x) / raydir.x;
  const float y1 = (0.5f - raypos.y) / raydir.y;
  const float z1 = (0.5f - raypos.z) / raydir.z;

  using namespace std;
  tmin = max(max(max(min(z0, z1), min(y0, y1)), min(x0, x1)), 0.0f);
  const float tmax = min(min(max(z0, z1), max(y0, y1)), max(x0, x1));
  return (tmin < tmax);
}

struct VolumeTextureExample : public circe::gl::BaseApp {
  VolumeTextureExample() : circe::gl::BaseApp(1600,
                                              800,
                                              "Volume Texture Example",
                                              circe::viewport_options::none,
                                              circe::app_options::no_viewport) {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // viewports_
    this->app->addViewport(0, 0, 1200, 800);
    this->app->addViewport(1200, 400, 400, 400);
    this->app->viewport(0).clear_screen_color = circe::Color::Black();
    this->app->viewport(1).clear_screen_color = circe::Color::Black();
    // DEBUG ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // domain transform
    auto domain_box = hermes::bbox3::unitBox(true);
    // wireframe
    w_box_model = circe::Shapes::box(domain_box, circe::shape_options::wireframe);
    HERMES_ASSERT_WITH_LOG(w_box_model.program.link(shaders_path, "color"), w_box_model.program.err)
    w_box_model.program.use();
    w_box_model.program.setUniform("color", circe::Color::Black());
    c_grid.color.a = 0.1;
    // SCENE ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // init box model and volume texture shader
    box_model = circe::Shapes::box(domain_box, circe::shape_options::uvw);
    HERMES_ASSERT_WITH_LOG(box_model.program.link(shaders_path, "woodcock_volume"), box_model.program.err)

    // init density texture
    circe::gl::Texture::Attributes attributes = {
        .size_in_texels = volume_resolution,
        .internal_format = GL_RED,
        .format = GL_RED,
        .type = GL_FLOAT,
        .target = GL_TEXTURE_3D,
    };
    t_density.set(attributes);
    circe::gl::Texture::View parameters(GL_TEXTURE_3D);
    t_density.bind();
    parameters.apply();
    // set density data
    std::vector<float> density(volume_resolution.total(), 0.f);
    hermes::range3 index_range(volume_resolution);
    auto domain_transform =
        hermes::Transform::translate(hermes::vec3(-0.5)) * hermes::Transform::scale(1.f / volume_resolution.width,
                                                                                    1.f / volume_resolution.height,
                                                                                    1.f / volume_resolution.depth);
    for (auto ijk :  index_range)
      density[index_range.flatIndex(ijk)] = getMenger(domain_transform(ijk), max_extinction);
    t_density.setTexels(density.data());
  }

  void prepareFrame() override {
    BaseApp::prepareFrame();
    camera_model.update(&app->viewport(1).camera());
    ImGuizmo::BeginFrame();
  }

  void render(circe::CameraInterface *camera) override {
    if (app->currentRenderingViewport() == 0)
      render0(camera);
    else if (app->currentRenderingViewport() == 1)
      render1(camera);
  }

  void render0(circe::CameraInterface *camera) {
    w_box_model.program.use();
    w_box_model.program.setUniform("projection", camera->getProjectionTransform());
    w_box_model.program.setUniform("model", camera->getModelTransform());
    w_box_model.program.setUniform("view", camera->getViewTransform());
    w_box_model.program.setUniform("color", circe::Color::White());

    w_box_model.draw();

    f32 t0 = 0;
    if (intersectVolumeBox(t0, app->viewport(1).camera().getPosition(), raydir))
      w_box_model.program.setUniform("color", circe::Color::Red());
    camera_rays.draw();

    c_grid.draw(camera);
    camera_model.draw(camera);

    // UI
    ImGui::Begin("controls");
    ImGui::InputInt("iterations", &max_iterations);
    ImGui::InputInt("interactions", &max_interactions);
    ImGui::SliderFloat("albedo", &albedo, 0, 1);
    ImGui::InputFloat("extinction", &max_extinction);
    ImGui::Text("%s", hermes::Str::concat(current_pixel).c_str());
    ImGui::Text("%s", hermes::Str::concat(t0).c_str());
    ImGui::Text("%s", hermes::Str::concat(l).c_str());

    ImGui::End();
  }

  void render1(circe::CameraInterface *camera) {
    t_density.bind(GL_TEXTURE0);
    HERMES_ASSERT(box_model.program.use())
    box_model.program.setUniform("projection", camera->getProjectionTransform());
    box_model.program.setUniform("model", camera->getModelTransform());
    box_model.program.setUniform("view", camera->getViewTransform());
    box_model.program.setUniform("extinction_tex", 0);
    box_model.program.setUniform("albedo", albedo);
    box_model.program.setUniform("max_iterations", max_iterations);
    box_model.program.setUniform("max_interactions", max_interactions);
    box_model.program.setUniform("max_extinction", max_extinction);

    box_model.program.setUniform("domain_box_lower", hermes::point3(-0.5));
    box_model.program.setUniform("domain_box_upper", hermes::point3(0.5));

    box_model.program.setUniform("camera.pos", camera->getPosition());
    box_model.program.setUniform("camera.up", hermes::normalize(camera->getUpVector()));
//    box_model.program.setUniform("camera.dir", hermes::normalize(camera->getDirection()));
    box_model.program.setUniform("camera.right", hermes::normalize(camera->getRight()));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    box_model.draw();

    ImGuizmo::SetRect(this->app->viewport(1).position().i,
                      0,
                      this->app->viewport(1).size().width,
                      this->app->viewport(1).size().height);
    circe::Gizmo::update(camera, gizmo_transform, ImGuizmo::OPERATION::TRANSLATE);

    auto next_pixel = app->viewport(1).project(gizmo_transform(hermes::point3()));
    if (next_pixel != current_pixel) {
      current_pixel = next_pixel;
      rand_state = hermes::vec2(current_pixel.i, current_pixel.j) + hermes::vec2(0.234525, 0.5663);

      auto p = app->viewport(1).viewCoordToNormDevCoord(hermes::point3(current_pixel.i, current_pixel.j, 0));
      p = app->viewport(1).unProject(p);
      raydir = hermes::normalize(p - app->viewport(1).camera().getPosition());
      std::vector<hermes::point3> path;

      l = traceVolume(app->viewport(1).camera().getPosition(), raydir, path);
      camera_rays = circe::Shapes::curve(path);
    }

  }

  // WOODCOCK ALGORITHM
  float getExtinction(hermes::point3 pos) const {
    if (!inVolume(pos)) {
      return max_extinction;
    }
    return getMenger(pos, max_extinction);
  }

  bool sampleInteraction(hermes::point3 &ray_pos, hermes::vec3 ray_dir) const {
    float t = 0.0f;
    hermes::point3 pos;
//    for (int step = 0; step < 100; ++step) {
    do {
      t -= log(1.0f - noise()) / max_extinction;
      pos = ray_pos + ray_dir * t;
      if (!inVolume(pos)) {
        return false;
      }
//      if (getExtinction(pos) >= noise() * max_extinction) {
//        break;
//      }
    } while (getExtinction(pos) < noise() * max_extinction);
//    }
    ray_pos = pos;
    return true;
  }

  hermes::vec3 traceVolume(hermes::point3 ray_pos, hermes::vec3 ray_dir, std::vector<hermes::point3> &path) const {
    path = {ray_pos};
    float t0 = 0;
    float w = 1.0f;
    if (intersectVolumeBox(t0, ray_pos, ray_dir)) {
      ray_pos += ray_dir * t0;
      int interaction_i = 0;
      for (; interaction_i < max_interactions; ++interaction_i) {
        path.emplace_back(ray_pos);
        if (!sampleInteraction(ray_pos, ray_dir)) {
          break;
        }
        w *= albedo;
        // Russian roulette absorption
        if (w < 0.2f) {
          if (noise() > w * 5.0f) {
            return hermes::vec3(0.0f, 0.0f, 0.0f);
          }
          w = 0.2f;
        }
        // Sample isotropic phase function
        const float phi = (2.0 * M_PI) * noise();
        const float cos_theta = 1.0f - 2.0f * noise();
        const float sin_theta = sqrt(1.0f - cos_theta * cos_theta);
        ray_dir = hermes::vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
      }
      // Is the path length exceeded?
      if (interaction_i >= max_interactions) {
        return hermes::vec3(0.0, 0.0, 0.0);
      }
    }
    const float f = (0.5f + 0.5f * ray_dir.y) * w;
    return hermes::vec3(f, f, f);
  }

  // parameters
  int max_iterations{10};
  int max_interactions{100};
  float max_extinction{100.f};
  float albedo{0.8f};

  // scene
  hermes::size3 volume_resolution{128, 128, 128};
  circe::gl::Texture t_density;
  circe::gl::SceneModel box_model;

  // debug
  circe::gl::SceneModel w_box_model;
  circe::gl::helpers::CartesianGrid c_grid;
  circe::gl::helpers::CameraModel camera_model;
  hermes::Transform gizmo_transform;
  circe::gl::SceneModel camera_rays;
  hermes::index2 current_pixel;
  // current ray
  hermes::vec3 raydir;
  hermes::vec3 l;
};

int main() {
  return VolumeTextureExample().run();
}

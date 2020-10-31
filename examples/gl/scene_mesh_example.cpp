// Created by filipecn on 2/25/18.
#include <circe/circe.h>

const char *vs = "#version 440 core\n"
                 // regular vertex attributes
                 "layout(location = 0) in vec3 position;"
                 "layout(location = 1) in vec3 normal;"
                 "layout(location = 2) in vec2 texcoord;"
                 // per instance attributes
                 "layout(location = 3) uniform mat4 proj;"
                 "layout(location = 4) uniform mat4 model;"
                 //       "uniform vec3 ldir;"
                 // output to fragment shader
                 "out VERTEX {"
                 //  "vec4 color;"
                 //       "vec3 normal;"
                 "vec2 uv;"
                 "} vertex;"
                 "void main() {"
                 "    gl_Position = proj * model * vec4(position,1);"
                 //  "    gl_Position = vec4(position,1);"
                 //   "    vertex.normal = normalize(model * vec4(normal,
                 //   0)).xyz;" "    float intensity = max(dot(vertex.normal,
                 //   (model * vec4(ldir,0)).xyz), 0.0);"
                 // "    vertex.color =   vec4(0,1,1,0.5) * intensity;"
                 "    vertex.uv = texcoord;"
                 "}";
const char *fs = "#version 440 core\n"
                 "layout(location = 5) uniform sampler2D tex;"
                 "in VERTEX {"
                 // " vec4 color;"
                 //" vec3 normal; "
                 "vec2 uv; } vertex;"
                 "layout(location = 0) out vec4 outColor;"
                 "void main() {"
                 //   "   outColor = vertex.color;"
                 " outColor = texture2D(tex, vertex.uv);"
                 //  "outColor = vec4(vertex.uv.x,0,vertex.uv.y,1);"
                 //  "outColor = vec4(0,0,0,1);"
                 "}";

int main() {
  circe::gl::SceneApp<> app(800, 800, "", false);
  app.init();
  app.addViewport(0, 0, 800, 800);
  ponos::RawMeshSPtr mesh(new ponos::RawMesh());
  // circe::loadOBJ("/mnt/windows/Users/fuiri/Desktop/dragon.obj", &mesh);
  circe::loadOBJ(
      "/mnt/windows/Projects/ponos/circe/examples/assets/torusknot.obj",
      mesh.get());
  mesh->apply(ponos::scale(0.1, 0.1, 0.1));
  mesh->computeBBox();
  mesh->splitIndexData();
  mesh->buildInterleavedData();
  auto texture = circe::gl::ImageTexture::checkBoard(64, 64);
  ponos::RawMeshSPtr m(
      ponos::create_icosphere_mesh(ponos::point3(), 1.f, 3, true, false));
  // create a vertex buffer for base mesh
  // circe::SceneMesh sm(*m.get());
  circe::gl::SceneMesh smesh(mesh.get());
  auto s = circe::gl::ShaderProgram(
      circe::gl::ShaderManager::instance().loadFromTexts(vs, nullptr, fs));
  s.addVertexAttribute("position", 0);
  s.addVertexAttribute("normal", 1);
  s.addVertexAttribute("texcoord", 2);
  s.addUniform("proj", 3);
  s.addUniform("model", 4);
  s.addUniform("tex", 5);
  // create a buffer for particles positions + sizes
  app.viewports[0].renderCallback = [&](const circe::CameraInterface *camera) {
    smesh.bind();
    smesh.vertexBuffer()->locateAttributes(s);
    texture.bind(GL_TEXTURE0);
    s.begin();
    s.setUniform("proj",
                 ponos::transpose(camera->getProjectionTransform().matrix()));
    s.setUniform("model",
                 ponos::transpose(camera->getViewTransform().matrix()));
    using namespace circe::gl;
    CHECK_GL_ERRORS;
    s.setUniform("tex", 0);
    //    s.setUniform("ldir", ponos::vec3(1,0,0));
    CHECK_GL_ERRORS;
    auto ib = smesh.indexBuffer();
    glDrawElements(ib->bufferDescriptor.element_type,
                   ib->bufferDescriptor.element_count *
                       ib->bufferDescriptor.element_size,
                   ib->bufferDescriptor.data_type, 0);
    CHECK_GL_ERRORS;
    s.end();
  };
  circe::gl::SceneObjectSPtr grid(new circe::gl::CartesianGrid(5));
  app.scene.add(grid.get());
  app.run();
  return 0;
}

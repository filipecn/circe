#include <circe/circe.h>

const char *bad_code = "#version 440 core\n"
                       "layout(location = 0) in vec3 position;\n"
                       "out vec3 fNormal;\n"
                       "out vec3 fPosition;\n"
                       "void main() {\n"
                       "  gl_Position = projection * view * model * vec4(position, 1.0);\n"
                       "}";

int main() {
  circe::gl::SceneApp<> app(800, 800, "");
  app.init();

  std::vector<circe::gl::Shader> shaders;
  shaders.emplace_back(ponos::FileSystem::readFile(std::string(SHADERS_PATH) + "/basic.vert"), GL_VERTEX_SHADER);
  shaders.emplace_back(ponos::FileSystem::readFile(std::string(SHADERS_PATH) + "/basic.frag"), GL_FRAGMENT_SHADER);

  circe::gl::Program program;
  program.attach(shaders);
  if (!program.link())
    std::cerr << "good program " << program.err << std::endl;

  // trying to compile with bad shader code
  circe::gl::Shader bad_shader(GL_VERTEX_SHADER, "");
  if (!bad_shader.compile(bad_code))
    std::cerr << bad_shader.err << std::endl;
  circe::gl::Program bad_program;
  bad_program.attach(bad_shader);
  if (!bad_program.link())
    std::cerr << "bad program " << bad_program.err << std::endl;
  circe::gl::Program p;
  p.destroy();
  return 0;
}

#include <circe/circe.h>

using namespace circe::gl;

const char *source =
    "#version 430\n"
    "layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;"
    "layout (std430, binding = 0) buffer PositionBuffer {\n"
    "\tvec4 positions[];\n"
    "};\n"
    "layout (std430, binding = 1) buffer VelocityBuffer {\n"
    "\tvec4 velocities[];\n"
    "};"
    "layout (std430, binding = 2) buffer FloatBuffer {\n"
    "\tfloat floats[];\n"
    "};\n"
    "void main()\n"
    "{\n"
    "uint index = gl_GlobalInvocationID.x;\n"
    "velocities[index] = vec4(index,0,2,0);\n"
    "positions[index] = velocities[index];\n"
    "floats[index] = 2;\n"
    \
"};\n";

#define PARTICLE_COUNT 1280000

typedef struct { GLfloat x, y, z, w; } Vec3;

Vec3 positions[PARTICLE_COUNT], velocities[PARTICLE_COUNT];
Vec3 np[PARTICLE_COUNT], nv[PARTICLE_COUNT];
float floats[PARTICLE_COUNT], nf[PARTICLE_COUNT];
int main() {
  SceneApp<> app(800, 800, "Compute Shader Example");
  StorageBuffer positionsBuffer(PARTICLE_COUNT * sizeof(Vec3), &positions[0]);
  StorageBuffer velocitiesBuffer(PARTICLE_COUNT * sizeof(Vec3), &velocities[0]);
  StorageBuffer floatsBuffer(PARTICLE_COUNT * sizeof(float), &floats[0]);
  floatsBuffer.bind();
  positionsBuffer.bind();
  velocitiesBuffer.bind();
  ComputeShader shader(source);
  shader.setGroupSize(ponos::size3(PARTICLE_COUNT / 128, 1, 1));
  shader.setBuffer("PositionBuffer", positionsBuffer.id(), 0);
  shader.setBuffer("VelocityBuffer", velocitiesBuffer.id(), 1);
  shader.setBuffer("FloatBuffer", floatsBuffer.id(), 2);
  shader.compute();
  positionsBuffer.read(&np[0]);
  velocitiesBuffer.read(&nv[0]);
  floatsBuffer.read(&nf[0]);
  std::cout << "results:\n";
  for (int i = 0; i < 10; i++) {
    std::cout << nv[i].x << " ";
    std::cout << nv[i].y << " ";
    std::cout << nv[i].z << std::endl;
  }
  for (int i = 0; i < 10; i++)
    std::cout << nf[i] << std::endl;
  app.run();
  return 0;
}

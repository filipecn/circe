#include <circe/gl/graphics/compute_shader.h>

#include <memory>

namespace circe::gl {

ComputeShader::ComputeShader(const char *source) {
  this->programId = static_cast<GLuint>(std::max(
      0, ShaderManager::instance().loadFromText(source, GL_COMPUTE_SHADER)));
  { // query up the workgroups
    int work_grp_size[3], work_grp_inv;
    // maximum global work group (total work in a dispatch)
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_size[2]);
    printf("max global (total) work group size x:%i y:%i z:%i\n",
           work_grp_size[0], work_grp_size[1], work_grp_size[2]);
    // maximum local work group (one shader's slice)
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
           work_grp_size[0], work_grp_size[1], work_grp_size[2]);
    // maximum compute shader invocations (x * y * z)
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max computer shader invocations %i\n", work_grp_inv);
  }
}

ComputeShader::ComputeShader(const TextureAttributes &a,
                             const TextureParameters &p, const char *source)
    : ComputeShader(source) {
  setTexture(a, p);
}

ComputeShader::~ComputeShader() = default;

bool ComputeShader::compute() {
  if (!ShaderProgram::begin())
    return false;
  if (texture)
    texture->bindImage(GL_TEXTURE0);
  for (unsigned int i = 0; i < blockIndices.size(); i++) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, blockIndices[i], bufferIds[i]);
    CHECK_GL_ERRORS;
  }
  glDispatchCompute(groupSize[0], groupSize[1], groupSize[2]);
  CHECK_GL_ERRORS;
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
  CHECK_GL_ERRORS;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  ShaderProgram::end();
  return true;
}

void ComputeShader::bindTexture(GLenum t) const { texture->bind(t); }
void ComputeShader::setBuffer(const char *name, GLuint id,
                              GLuint bindingPoint) {
  UNUSED_VARIABLE(name);
  // blockIndices.emplace_back(glGetProgramResourceIndex(programId,
  // GL_SHADER_STORAGE_BLOCK, name));
  blockIndices.emplace_back(bindingPoint);
  CHECK_GL_ERRORS;
  bufferIds.emplace_back(id);
}
void ComputeShader::setTexture(const TextureAttributes &a,
                               const TextureParameters &p) {
  texture = std::make_unique<Texture>(a, p);
  groupSize = texture->size();
}
void ComputeShader::setGroupSize(const ponos::size3 gs) { groupSize = gs; }

} // namespace circe

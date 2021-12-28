#ifndef CIRCE_UTILS_OPEN_GL_H
#define CIRCE_UTILS_OPEN_GL_H

#define GL_DEBUG
//#define GLEW_BUILD
//#define GLEW_STATIC
//#include <GL/glew.h>
#if !defined(GLAD_ALREADY_INCLUDED)
#include <glad/glad.h>
#endif // GLAD_ALREADY_INCLUDED

//#include <vulkan/vulkan.h>
//#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

//#if defined(NANOGUI_GLAD)
//#if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
//#define GLAD_GLAPI_EXPORT
//#endif
//#else
//#if defined(__APPLE__)
//#define GLFW_INCLUDE_GLCOREARB
//#else
//#define GL_GLEXT_PROTOTYPES
//#endif
//#endif

#include <circe/colors/color.h>
#include <hermes/data_structures/raw_mesh.h>
#include <csignal>

namespace circe::gl {

class OpenGL final {
public:
  static size_t primitiveCount(GLuint primitive_type, GLuint index_count) {
    size_t primitive_count = 0;
    switch (primitive_type) {
    case GL_POINTS: primitive_count = index_count;
      break;
    case GL_TRIANGLES: primitive_count = index_count / 3;
      break;
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN: primitive_count = index_count - 2;
      break;
    case GL_LINES:primitive_count = index_count / 2;
      break;
    case GL_LINE_STRIP: primitive_count = index_count - 1;
      break;
    case GL_LINE_LOOP: primitive_count = index_count;
      break;
    case GL_QUADS: primitive_count = index_count / 4;
      break;
    default: hermes::Log::warn("Element type considered GL_POINTS on index buffer assignment.");
      primitive_count = index_count;
    }
    return primitive_count;
  }
  static std::string PrimitiveToStr(hermes::GeometricPrimitiveType p) {
#define ES(P) \
    if(p == P) return #P
    ES(hermes::GeometricPrimitiveType::POINTS);
    ES(hermes::GeometricPrimitiveType::LINES);
    ES(hermes::GeometricPrimitiveType::LINE_STRIP);
    ES(hermes::GeometricPrimitiveType::LINE_LOOP);
    ES(hermes::GeometricPrimitiveType::TRIANGLES);
    ES(hermes::GeometricPrimitiveType::TRIANGLE_STRIP);
    ES(hermes::GeometricPrimitiveType::TRIANGLE_FAN);
    ES(hermes::GeometricPrimitiveType::QUADS);
    ES(hermes::GeometricPrimitiveType::TETRAHEDRA);
    ES(hermes::GeometricPrimitiveType::CUSTOM);
    return "ERR";
#undef ES
  }
  static GLuint PrimitiveToGL(hermes::GeometricPrimitiveType primitive) {
#define RET_GL(P, G) \
  if(primitive == P) \
  return G;
    RET_GL(hermes::GeometricPrimitiveType::POINTS, GL_POINTS)
    RET_GL(hermes::GeometricPrimitiveType::LINES, GL_LINES)
    RET_GL(hermes::GeometricPrimitiveType::LINE_STRIP, GL_LINE_STRIP)
    RET_GL(hermes::GeometricPrimitiveType::LINE_LOOP, GL_LINE_LOOP)
    RET_GL(hermes::GeometricPrimitiveType::TRIANGLES, GL_TRIANGLES)
    RET_GL(hermes::GeometricPrimitiveType::TRIANGLE_STRIP, GL_TRIANGLE_STRIP)
    RET_GL(hermes::GeometricPrimitiveType::TRIANGLE_FAN, GL_TRIANGLE_FAN)
    RET_GL(hermes::GeometricPrimitiveType::QUADS, GL_QUADS)
    return GL_POINTS;
#undef RET_GL
  }
  static std::string EnumToStr(GLenum e) {
#define RET_STR(A) \
    if(e == A) return #A;
    RET_STR(GL_TEXTURE_1D)
    RET_STR(GL_TEXTURE_2D)
    RET_STR(GL_TEXTURE_3D)
    RET_STR(GL_TEXTURE_1D_ARRAY)
    RET_STR(GL_TEXTURE_2D_ARRAY)
    RET_STR(GL_TEXTURE_RECTANGLE)
    RET_STR(GL_TEXTURE_CUBE_MAP)
    RET_STR(GL_TEXTURE_CUBE_MAP_ARRAY)
    RET_STR(GL_TEXTURE_BUFFER)
    RET_STR(GL_TEXTURE_2D_MULTISAMPLE)
    RET_STR(GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
    RET_STR(GL_BYTE)
    RET_STR(GL_UNSIGNED_BYTE)
    RET_STR(GL_SHORT)
    RET_STR(GL_UNSIGNED_SHORT)
    RET_STR(GL_HALF_FLOAT)
    RET_STR(GL_INT)
    RET_STR(GL_UNSIGNED_INT)
    RET_STR(GL_FIXED)
    RET_STR(GL_FLOAT)
    RET_STR(GL_DOUBLE)
    RET_STR(GL_NAME_LENGTH)
    RET_STR(GL_BUFFER_BINDING)
    RET_STR(GL_BUFFER_DATA_SIZE)
    RET_STR(GL_NUM_ACTIVE_VARIABLES)
    RET_STR(GL_ACTIVE_VARIABLES)
    RET_STR(GL_TYPE)
    RET_STR(GL_ARRAY_SIZE)
    RET_STR(GL_OFFSET)
    RET_STR(GL_BLOCK_INDEX)
    RET_STR(GL_ARRAY_STRIDE)
    RET_STR(GL_MATRIX_STRIDE)
    RET_STR(GL_IS_ROW_MAJOR)
    RET_STR(GL_ATOMIC_COUNTER_BUFFER_INDEX)
    RET_STR(GL_REFERENCED_BY_VERTEX_SHADER)
    RET_STR(GL_REFERENCED_BY_FRAGMENT_SHADER)
    RET_STR(GL_REFERENCED_BY_GEOMETRY_SHADER)
    RET_STR(GL_REFERENCED_BY_TESS_CONTROL_SHADER)
    RET_STR(GL_REFERENCED_BY_TESS_EVALUATION_SHADER)
    RET_STR(GL_REFERENCED_BY_COMPUTE_SHADER)
    RET_STR(GL_LOCATION)
    RET_STR(GL_UNIFORM_TYPE)
    RET_STR(GL_UNIFORM_SIZE)
    RET_STR(GL_UNIFORM_NAME_LENGTH)
    RET_STR(GL_UNIFORM_BLOCK_INDEX)
    RET_STR(GL_UNIFORM_OFFSET)
    RET_STR(GL_UNIFORM_ARRAY_STRIDE)
    RET_STR(GL_UNIFORM_MATRIX_STRIDE)
    RET_STR(GL_UNIFORM_IS_ROW_MAJOR)
    RET_STR(GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX)
    RET_STR(GL_UNIFORM_BLOCK_BINDING)
    RET_STR(GL_UNIFORM_BLOCK_DATA_SIZE)
    RET_STR(GL_UNIFORM_BLOCK_NAME_LENGTH)
    RET_STR(GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS)
    RET_STR(GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES)
    RET_STR(GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER)
    RET_STR(GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER)
    RET_STR(GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER)
    RET_STR(GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER)
    RET_STR(GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER)
    RET_STR(GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER)
    return "";
#undef RET_STR
  }

  static std::string TypeToStr(GLenum e, std::string *glsl_name = nullptr) {
#define RET_STR(A, B) \
    if(e == A) {      \
    if(glsl_name)     \
      *glsl_name = #B;\
    return #A;}
    RET_STR(GL_FLOAT, float)
    RET_STR(GL_FLOAT_VEC2, vec2)
    RET_STR(GL_FLOAT_VEC3, vec3)
    RET_STR(GL_FLOAT_VEC4, vec4)
    RET_STR(GL_DOUBLE, double)
    RET_STR(GL_DOUBLE_VEC2, dvec2)
    RET_STR(GL_DOUBLE_VEC3, dvec3)
    RET_STR(GL_DOUBLE_VEC4, dvec4)
    RET_STR(GL_INT, int)
    RET_STR(GL_INT_VEC2, ivec2)
    RET_STR(GL_INT_VEC3, ivec3)
    RET_STR(GL_INT_VEC4, ivec4)
    RET_STR(GL_UNSIGNED_INT, unsigned int)
    RET_STR(GL_UNSIGNED_INT_VEC2, uvec2)
    RET_STR(GL_UNSIGNED_INT_VEC3, uvec3)
    RET_STR(GL_UNSIGNED_INT_VEC4, uvec4)
    RET_STR(GL_BOOL, bool)
    RET_STR(GL_BOOL_VEC2, bvec2)
    RET_STR(GL_BOOL_VEC3, bvec3)
    RET_STR(GL_BOOL_VEC4, bvec4)
    RET_STR(GL_FLOAT_MAT2, mat2)
    RET_STR(GL_FLOAT_MAT3, mat3)
    RET_STR(GL_FLOAT_MAT4, mat4)
    RET_STR(GL_FLOAT_MAT2x3, mat2x3)
    RET_STR(GL_FLOAT_MAT2x4, mat2x4)
    RET_STR(GL_FLOAT_MAT3x2, mat3x2)
    RET_STR(GL_FLOAT_MAT3x4, mat3x4)
    RET_STR(GL_FLOAT_MAT4x2, mat4x2)
    RET_STR(GL_FLOAT_MAT4x3, mat4x3)
    RET_STR(GL_DOUBLE_MAT2, dmat2)
    RET_STR(GL_DOUBLE_MAT3, dmat3)
    RET_STR(GL_DOUBLE_MAT4, dmat4)
    RET_STR(GL_DOUBLE_MAT2x3, dmat2x3)
    RET_STR(GL_DOUBLE_MAT2x4, dmat2x4)
    RET_STR(GL_DOUBLE_MAT3x2, dmat3x2)
    RET_STR(GL_DOUBLE_MAT3x4, dmat3x4)
    RET_STR(GL_DOUBLE_MAT4x2, dmat4x2)
    RET_STR(GL_DOUBLE_MAT4x3, dmat4x3)
    RET_STR(GL_SAMPLER_1D, sampler1D)
    RET_STR(GL_SAMPLER_2D, sampler2D)
    RET_STR(GL_SAMPLER_3D, sampler3D)
    RET_STR(GL_SAMPLER_CUBE, samplerCube)
    RET_STR(GL_SAMPLER_1D_SHADOW, sampler1DShadow)
    RET_STR(GL_SAMPLER_2D_SHADOW, sampler2DShadow)
    RET_STR(GL_SAMPLER_1D_ARRAY, sampler1DArray)
    RET_STR(GL_SAMPLER_2D_ARRAY, sampler2DArray)
    RET_STR(GL_SAMPLER_1D_ARRAY_SHADOW, sampler1DArrayShadow)
    RET_STR(GL_SAMPLER_2D_ARRAY_SHADOW, sampler2DArrayShadow)
    RET_STR(GL_SAMPLER_2D_MULTISAMPLE, sampler2DMS)
    RET_STR(GL_SAMPLER_2D_MULTISAMPLE_ARRAY, sampler2DMSArray)
    RET_STR(GL_SAMPLER_CUBE_SHADOW, samplerCubeShadow)
    RET_STR(GL_SAMPLER_BUFFER, samplerBuffer)
    RET_STR(GL_SAMPLER_2D_RECT, sampler2DRect)
    RET_STR(GL_SAMPLER_2D_RECT_SHADOW, sampler2DRectShadow)
    RET_STR(GL_INT_SAMPLER_1D, isampler1D)
    RET_STR(GL_INT_SAMPLER_2D, isampler2D)
    RET_STR(GL_INT_SAMPLER_3D, isampler3D)
    RET_STR(GL_INT_SAMPLER_CUBE, isamplerCube)
    RET_STR(GL_INT_SAMPLER_1D_ARRAY, isampler1DArray)
    RET_STR(GL_INT_SAMPLER_2D_ARRAY, isampler2DArray)
    RET_STR(GL_INT_SAMPLER_2D_MULTISAMPLE, isampler2DMS)
    RET_STR(GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, isampler2DMSArray)
    RET_STR(GL_INT_SAMPLER_BUFFER, isamplerBuffer)
    RET_STR(GL_INT_SAMPLER_2D_RECT, isampler2DRect)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_1D, usampler1D)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_2D, usampler2D)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_3D, usampler3D)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_CUBE, usamplerCube)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, usampler1DArray)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, usampler2DArray)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, usampler2DMS)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, usampler2DMSArray)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_BUFFER, usamplerBuffer)
    RET_STR(GL_UNSIGNED_INT_SAMPLER_2D_RECT, usampler2DRect)
    return "";
#undef RET_STR
  }

  [[nodiscard]] static u64 dataSizeInBytes(GLuint data_type) {
    switch (data_type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE: return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT: return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FIXED:
    case GL_FLOAT:return 4;
    case GL_DOUBLE: return 8;
    default: return 0;
    }
    return 0;
  }
  template<typename T>
  static GLenum dataTypeEnum() {
    if (std::is_same_v<T, i32>)
      return GL_INT;
    if (std::is_same_v<T, u32>)
      return GL_UNSIGNED_INT;
    if (std::is_same_v<T, f32>)
      return GL_FLOAT;
    if (std::is_same_v<T, f64>)
      return GL_DOUBLE;
    if (std::is_same_v<T, u8>)
      return GL_UNSIGNED_BYTE;
    if (std::is_same_v<T, i8>)
      return GL_BYTE;
    if (std::is_same_v<T, i16>)
      return GL_SHORT;
    if (std::is_same_v<T, u16>)
      return GL_UNSIGNED_SHORT;
//    if (std::is_same_v<T, f16>)
//      return GL_HALF_FLOAT;
    return GL_FIXED;
  }
  static std::string primitiveGLName(GLuint primitive_type) {
#define RET_STR(A) \
    if(primitive_type == A) {      \
    return #A;}
    RET_STR(GL_POINTS);
    RET_STR(GL_LINES);
    RET_STR(GL_LINE_LOOP);
    RET_STR(GL_LINE_STRIP);
    RET_STR(GL_TRIANGLES);
    RET_STR(GL_TRIANGLE_STRIP);
    RET_STR(GL_TRIANGLE_FAN);
    RET_STR(GL_LINES_ADJACENCY);
    RET_STR(GL_LINE_STRIP_ADJACENCY);
    RET_STR(GL_TRIANGLES_ADJACENCY);
    RET_STR(GL_TRIANGLE_STRIP_ADJACENCY);
    RET_STR(GL_QUADS);
#undef RET_STR
    return "";
  }
  static GLenum dataTypeEnum(hermes::DataType T) {
#define MATCH_TYPE(TT, R) \
   if(hermes::DataType::TT == T)  \
   return R;
    MATCH_TYPE(I8, GL_BYTE)
    MATCH_TYPE(U8, GL_UNSIGNED_BYTE)
    MATCH_TYPE(I16, GL_SHORT)
    MATCH_TYPE(U16, GL_UNSIGNED_SHORT)
    MATCH_TYPE(F16, GL_HALF_FLOAT)
    MATCH_TYPE(I32, GL_INT)
    MATCH_TYPE(U32, GL_UNSIGNED_INT)
    MATCH_TYPE(I32, GL_FIXED)
    MATCH_TYPE(F32, GL_FLOAT)
    MATCH_TYPE(F64, GL_DOUBLE)
#undef MATCH_TYPE
    return GL_FIXED;
  }
};

#ifdef GL_DEBUG

#define CHECK_GL(A)                                                            \
  {                                                                            \
    A;                                                                         \
    if (checkGL(__FILE__, __LINE__, __FUNCTION__, #A))                         \
      raise(SIGSEGV);                                                          \
  }
#define CHECK_GL_ERRORS                                                        \
  if (checkGL(__FILE__, __LINE__, __FUNCTION__))                               \
  raise(SIGSEGV);
#define CHECK_FRAMEBUFFER checkFramebuffer(__FILE__, __LINE__, __FUNCTION__)
#else
#define CHECK_GL
#define CHECK_GL_ERRORS
#define CHECK_FRAMEBUFFER
#endif

inline void glfwError(int id, const char *description) {
  HERMES_UNUSED_VARIABLE(id);
  std::cerr << description << std::endl;
}

bool initGLEW();

/* info
 * @shader **[in]** shader id
 * Print out the information log for a shader object
 */
void printShaderInfoLog(GLuint shader);

/* info
 * @program **[in]** program id
 * Print out the information log for a program object
 */
void printProgramInfoLog(GLuint program);
/// \param file caller file
/// \param line_number caller line number
/// \param function caller function
/// \param line caller line
/// \return true if any OpenGL error occurred
bool checkGL(const char *file, int line_number, const char *function = nullptr,
             const char *line = nullptr);
///
/// \param error
/// \return string containing error description
std::string glErrorToString(GLenum error, bool description = true);

/* error check
 * Check framebuffer is COMPLETE
 * @return **false** if NO errors occured
 */
bool checkFramebuffer(const char *file, int line_number, const char *function = nullptr);

/* query
 * @major **[out]** receives major version
 * @minor **[out]** receives minor version
 * Retreives opengl version. Any error is sent to **stderr**.
 */
void getGlVersion(int *major, int *minor);

void glColor(Color c);

/// multiplies **t** to current OpenGL matrix
/// \param transform
void glApplyTransform(const hermes::Transform &transform);

hermes::Transform glGetProjectionTransform();

hermes::Transform glGetModelviewTransform();

hermes::Transform glGetMVPTransform();

} // namespace circe

#endif // CIRCE_UTILS_OPEN_GL_H

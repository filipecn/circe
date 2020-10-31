/*
 * Copyright (c) 2017 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef CIRCE_GRAPHICS_SHADER_MANAGER_H
#define CIRCE_GRAPHICS_SHADER_MANAGER_H

#include <circe/gl/utils/open_gl.h>

#include <initializer_list>
#include <map>
#include <stdarg.h>
#include <string>
#include <vector>

namespace circe::gl {

/// \brief singleton
/// Manages shader programs
class ShaderManager {
public:
  static ShaderManager &instance() { return instance_; }
  virtual ~ShaderManager() = default;
  /// \brief Creates a shader program from a list of shader files.
  /// It expects only one file of each type with extensions .frag, .vert, etc...
  ///\return program id. **-1** if error.
  static int loadFromFiles(std::initializer_list<const char *> l);
  /// \brief Creates a shader program from strings.
  /// \param vs vertex shader
  /// \param gs geometry shader
  /// \param fs fragment shader
  /// \return program id. **-1** if error.
  static int loadFromTexts(const char *vs, const char *gs, const char *fs);
  static int loadFromText(const char *s, GLuint shaderType);
  /// \brief use program
  ///\param program **[in]** program's id
  /// Activate program
  ///\return **true** if success
  static bool useShader(GLuint program);

  ShaderManager(ShaderManager const &) = delete;
  void operator=(ShaderManager const &) = delete;

private:
  ShaderManager();

  static GLuint createProgram(const GLchar *, const GLchar *);
  static GLuint compile(const char *shaderSource, GLuint shaderType);
  static GLuint createProgram(const std::vector<GLuint> &objects);

  static ShaderManager instance_;
};

#define CIRCE_NO_VAO_VS                                                        \
  "#version 440 core\n out vec2 texCoord;"                                     \
  "void main() {"                                                              \
  "    float x = -1.0 + float((gl_VertexID & 1) << 2);"                        \
  "    float y = -1.0 + float((gl_VertexID & 2) << 1);"                        \
  "    texCoord.x = (x+1.0)*0.5;"                                              \
  "    texCoord.y = (y+1.0)*0.5;"                                              \
  "    gl_Position = vec4(x, y, 0, 1);"                                        \
  "}"

#define CIRCE_NO_VAO_FS                                                        \
  "#version 440 core\n"                                                        \
  "out vec4 outColor;"                                                         \
  "in vec2 texCoord;"                                                          \
  "uniform sampler2D tex;"                                                     \
  "void main() {"                                                              \
  "     outColor = texture(tex, texCoord);"                                    \
  "}"

#define CIRCE_INSTANCES_VS                                                     \
  "#version 440 core\n"                                                        \
  "layout (location = 0) in vec3 position;"                                    \
  "layout (location = 1) in vec4 color;"                                       \
  "layout (location = 2) in mat4 transform_matrix;"                            \
  "layout (location = 3) uniform mat4 model_view_matrix;"                      \
  "layout (location = 4) uniform mat4 projection_matrix;"                      \
  "out VERTEX {"                                                               \
  "     vec4 color;"                                                           \
  "} vertex;"                                                                  \
  "void main() {"                                                              \
  "    gl_Position = projection_matrix * model_view_matrix * "                 \
  "transform_matrix "                                                          \
  "*    vec4(position,1);"                                                     \
  "    vertex.color = color;"                                                  \
  "}";

#define CIRCE_INSTANCES_FS                                                     \
  "#version 440 core\n"                                                        \
  "in VERTEX { vec4 color; } vertex;"                                          \
  "out vec4 outColor;"                                                         \
  "void main() {"                                                              \
  "   outColor = vertex.color;"                                                \
  "}";

} // namespace circe

#endif // CIRCE_GRAPHICS_SHADER_MANAGER_H

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

#ifndef CIRCE_GRAPHICS_SHADER_H
#define CIRCE_GRAPHICS_SHADER_H

#include <circe/colors/color.h>
#include <circe/gl/graphics/shader_manager.h>
#include <circe/gl/io/buffer.h>

#include <ponos/ponos.h>

#include <initializer_list>
#include <set>

namespace circe::gl {

// Uniquely holds a single shader open gl object (RAII)
// Note: This object can't be copied, only moved
class Shader {
public:
  friend class Program;
  Shader();
  /// \param type shader type
  explicit Shader(GLuint type, const std::string &code);
  /// \param code
  /// \param type
  Shader(const ponos::Path &code, GLuint type);
  Shader(const Shader &other) = delete;
  Shader(const Shader &&other) = delete;
  /// Copy constructor
  Shader(Shader &other);
  // Assign constructor
  Shader(Shader &&other) noexcept;
  /// The destructor destroys the open gl object
  ~Shader();
  /// \param type shader type: GL_COMPUTE_SHADER, GL_VERTEX_SHADER,
  /// GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
  /// GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER
  void setType(GLuint type);
  ///
  /// \return
  [[nodiscard]] GLuint type() const;
  ///
  /// \param code
  /// \return
  bool compile(const std::string &code);
  ///
  /// \param code
  /// \param type
  bool compile(const std::string &code, GLuint type);
  ///
  /// \param file
  /// \param type
  bool compile(const ponos::Path &file, GLuint type);
  ///
  /// \return
  [[nodiscard]] GLuint id() const;

  std::string err; //!< compilation error messages (if any)

private:
  GLuint type_{0};
  GLuint id_{0};
};

// Uniquely holds a shader program (RAII)
// A set of shaders compiled into a single program
// Note: This object can't be copied, only moved
class Program {
public:
  struct Uniform {
    std::string name;
    u64 index; //!< uniform index
    GLint type; //!< GL_TYPE
    GLint offset; //!< GL_OFFSET
    GLint block_index; //!< GL_BLOCK_INDEX
    GLint array_size; //!<  GL_ARRAY_SIZE
    GLint array_stride; //!< GL_ARRAY_STRIDE
    GLint matrix_stride; //!< GL_MATRIX_STRIDE
    GLint is_row_major; //!< GL_IS_ROW_MAJOR
    GLint location; //!< GL_LOCATION
  };
  struct UniformBlock {
    std::string name;
    GLuint index; //!< block index
    GLuint buffer_binding; //!< GL_BUFFER_BINDING
    GLuint size_in_bytes; //!< GL_BUFFER_DATA_SIZE
    std::vector<GLint> variable_indices; //!< GL_ACTIVE_VARIABLES
  };

  Program();
  /// \param files expect extensions: .frag, .vert
  explicit Program(const std::vector<ponos::Path> &files);
  /// \param files expect extensions: .frag, .vert
  Program(std::initializer_list<ponos::Path> files);
  /// Construct from list of shaders
  /// \param shader_list
  explicit Program(const std::vector<Shader> &shader_list);
  Program(const Program &other) = delete;
  /// Copy constructor
  /// \param other
  Program(Program &other);
  /// Assign constructor
  /// \param other
  Program(Program &&other) noexcept;
  ~Program();
  /// Calls glDeleteProgram, but does not clean attributes and uniforms
  /// Note: Shaders must be attached and linked again for reuse
  void destroy();
  /// Attach shader (calls glAttachShader)
  /// \param shader pre-compiled shader
  void attach(const Shader &shader);
  /// Attach shader list (calls glAttachShader for each shader)
  /// \param shader_list pre-compiled shader list
  void attach(const std::vector<Shader> &shader_list);
  /// Link pre-attached shaders
  /// \return
  bool link();
  /// Seach for files matching folder/shader_name.ext, where the values tried
  /// for ext are frag, vert, ...
  /// \param folder path/to/folder containing shaders
  /// \param shader_name shader name without extension
  /// \return
  bool link(const ponos::Path& folder, const std::string& shader_name);
  /// Attach and create program
  /// \param shader_list pre-compiled shader list
  /// \return
  bool link(const std::vector<Shader> &shader_list);
  /// Attach and create program
  /// \param shader_file_list files expect extensions: .frag, .vert
  /// \return
  bool link(const std::vector<ponos::Path> &shader_file_list);
  /// Activate program (tries to link if necessary)
  /// \return
  bool use();
  /// Register attribute from shader code
  /// \param name attribute name
  /// \param location layout location (must match shader code)
  [[deprecated]]
  void addVertexAttribute(const std::string &name, GLint location);
  /// Register uniform from shader code
  /// \param name uniform name
  /// \param location layout location (must match shader code)
  [[deprecated]]
  void addUniform(const std::string &name, GLint location);
  /// locates attribute **name** in shader's program
  /// \param name attribute's name
  /// \return attributes layout location (-1 if not found)
  [[nodiscard]] int locateAttribute(const std::string &name) const;
  ///
  /// \return
  [[nodiscard]] GLuint id() const;
  /// \return
  [[nodiscard]] inline const std::vector<Uniform> &uniforms() const { return uniforms_; }
  /// \return
  [[nodiscard]] inline const std::vector<UniformBlock> &uniformBlocks() const { return uniform_blocks_; }
  // Uniforms
  void setUniform(const std::string &name, const ponos::Transform &t);
  void setUniform(const std::string &name, const ponos::mat4 &m);
  void setUniform(const std::string &name, const ponos::mat3 &m);
  void setUniform(const std::string &name, const ponos::vec4 &v);
  void setUniform(const std::string &name, const ponos::vec3 &v);
  void setUniform(const std::string &name, const ponos::vec2 &v);
  void setUniform(const std::string &name, const ponos::point3 &v);
  void setUniform(const std::string &name, const Color &c);
  void setUniform(const std::string &name, int i);
  void setUniform(const std::string &name, float f);
  [[nodiscard]] bool hasUniform(const std::string &name) const;
  // Uniform Blocks
  void setUniformBlockBinding(const std::string &name, GLuint buffer_binding);

  friend std::ostream &operator<<(std::ostream &o, const Program &program);

  std::string err; //!< linkage errors (if any)
private:
  bool checkLinkageErrors();
  GLint getUniLoc(const std::string &name);
  void create();
  /// Cache uniform and attribute layout locations
  void cacheLocations();

  GLuint id_{0};
  std::map<std::string, GLint> attr_locations_;
  std::map<std::string, GLint> uniform_locations_;
  std::map<std::string, u64> ub_map_name_id_;
  std::vector<Uniform> uniforms_;
  std::vector<UniformBlock> uniform_blocks_;
  bool linked_{false};
};

/// Holds a program id and serves as an interface for setting its uniforms.
class ShaderProgram {
public:
  explicit ShaderProgram(int id = 0);
  ShaderProgram(const ShaderProgram &other);
  ShaderProgram(const ShaderProgram &&other) noexcept;
  ShaderProgram &operator=(const ShaderProgram &other);

  /// It expects only one file of each type with extensions .fs, .vs and .gs.
  /// \brief Creates a shader program from strings.
  /// \param vs vertex shader
  /// \param gs geometry shader
  /// \param fs fragment shader
  ShaderProgram(const char *vs, const char *gs, const char *fs);
  /// It expects only one file of each type with extensions .fs, .vs and .gs.
  ShaderProgram(std::initializer_list<const char *> files);
  /// \brief Creates a shader program from shader files.
  /// It expects only one file of each type with extensions .fs, .vs and .gs.
  /// \return program id. **-1** if error.
  bool loadFromFiles(std::initializer_list<const char *> files);
  /// Activate shader program
  bool begin();
  /// Deactivate shader program
  static void end();
  /// \param name
  /// \param location
  void addVertexAttribute(const char *name, GLint location);
  ///
  /// \param name
  /// \param location
  void addUniform(const std::string &name, GLint location);
  /// locates atribute **name** in shader's program
  /// \param name attibute's name, must match name in shader code
  /// \return attributes layout location
  [[nodiscard]] int locateAttribute(const std::string &name) const;
  /// Register shader attributes into vertex buffer
  /// \param b buffer pointer (must match attribute names)
  void registerVertexAttributes(const GLVertexBuffer *b);
  // Uniforms
  void setUniform(const char *name, const ponos::Transform &t);
  void setUniform(const char *name, const ponos::mat4 &m);
  void setUniform(const char *name, const ponos::mat3 &m);
  void setUniform(const char *name, const ponos::vec4 &v);
  void setUniform(const char *name, const ponos::vec3 &v);
  void setUniform(const char *name, const ponos::vec2 &v);
  void setUniform(const char *name, const ponos::point3 &v);
  void setUniform(const char *name, const Color &c);
  void setUniform(const char *name, int i);
  void setUniform(const char *name, float f);

  friend std::ostream &operator<<(std::ostream &o, const ShaderProgram &shader) {
    o << "SHADER (programId " << shader.programId << ")\n";
    o << "vertex attributes:\n";
    for (const auto &a : shader.attrLocations)
      o << "\t" << a.first << "\t" << a.second << std::endl;
    o << "uniform list:\n";
    for (const auto &a : shader.uniformLocations)
      o << "\t" << a.first << "\t" << a.second << std::endl;
    o << std::endl;
    return o;
  }

protected:
  GLuint programId = 0;

  //  std::set<const char *> vertexAttributes;
  std::map<std::string, GLint> attrLocations;
  std::map<std::string, GLint> uniformLocations;

  GLint getUniLoc(const GLchar *name);
};

typedef std::shared_ptr<ShaderProgram> ShaderProgramPtr;

template<typename... TArg>
ShaderProgramPtr createShaderProgramPtr(TArg &&... Args) {
  return std::make_shared<ShaderProgram>(std::forward<TArg>(Args)...);
}

} // namespace circe

#endif // CIRCE_GRAPHICS_SHADER_H

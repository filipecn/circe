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

#include <circe/gl/graphics/shader_manager.h>

namespace circe::gl {

ShaderManager ShaderManager::instance_;

ShaderManager::ShaderManager() = default;

int ShaderManager::loadFromFiles(std::initializer_list<const char *> l) {
  std::vector<GLuint> objects;
  GLuint types[] = {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER,
                    GL_COMPUTE_SHADER};
  for (auto v : l) {
    std::string filename(v);
    if (filename.size() < 4)
      continue;
    char *source = nullptr;
    std::cout << "loading shader file " << filename << std::endl;
    if (!ponos::readFile(filename.c_str(), &source))
      continue;
    GLuint shaderType = 0;
    switch (filename[filename.size() - 4]) {
    case 'v':
      shaderType = 0;
      break;
    case 'g':
      shaderType = 1;
      break;
    case 'f':
      shaderType = 2;
      break;
    case 'c':
      shaderType = 3;
      break;
    default:
      continue;
    }
    objects.emplace_back(compile(source, types[shaderType]));
    if (source)
      free(source);
  }
  if (objects.empty())
    return -1;
  GLuint program = createProgram(objects);
  if (!program)
    return -1;
  return static_cast<int>(program);
}

int ShaderManager::loadFromTexts(const char *vs, const char *gs,
                                 const char *fs) {
  std::vector<GLuint> objects(3, 0);
  if (vs != nullptr)
    objects[0] = compile(vs, GL_VERTEX_SHADER);
  if (gs != nullptr)
    objects[1] = compile(gs, GL_GEOMETRY_SHADER);
  if (fs != nullptr)
    objects[2] = compile(fs, GL_FRAGMENT_SHADER);
  GLuint program = createProgram(objects);
  if (!program)
    return -1;
  return static_cast<int>(program);
}

int ShaderManager::loadFromText(const char *s, GLuint shaderType) {
  std::vector<GLuint> object(1, 0);
  if (!s)
    return -1;
  object[0] = compile(s, shaderType);
  GLuint program = createProgram(object);
  if (!program)
    return -1;
  return static_cast<int>(program);
}

bool ShaderManager::useShader(GLuint program) {
  CHECK_GL(glUseProgram(program));
  return true;
}

GLuint ShaderManager::createProgram(const GLchar *vertexShaderSource,
                                    const GLchar *fragmentShaderSource) {
  GLuint ProgramObject;             // handles to objects
  GLint vertCompiled, fragCompiled; // status values
  GLint linked;

  // Create a vertex shader object and a fragment shader object
  GLuint VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  // Load source code strings into shaders
  glShaderSource(VertexShaderObject, 1, &vertexShaderSource, nullptr);
  glShaderSource(FragmentShaderObject, 1, &fragmentShaderSource, nullptr);
  // Compile the brick vertex shader, and print out
  // the compiler log file.
  glCompileShader(VertexShaderObject);
  CHECK_GL_ERRORS;
  glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &vertCompiled);
  if (!vertCompiled)
    printShaderInfoLog(VertexShaderObject);
  // Compile the brick vertex shader, and print out
  // the compiler log file.
  glCompileShader(FragmentShaderObject);
  CHECK_GL_ERRORS;
  glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &fragCompiled);
  if (!fragCompiled)
    printShaderInfoLog(FragmentShaderObject);
  if (!vertCompiled || !fragCompiled) {
    std::cerr << "couldn't compile shader!\n";
    return 0;
  }
  // Create a program object and attach the two compiled shaders
  ProgramObject = glCreateProgram();
  glAttachShader(ProgramObject, VertexShaderObject);
  glAttachShader(ProgramObject, FragmentShaderObject);
  // Link the program object and print out the info log
  glLinkProgram(ProgramObject);
  CHECK_GL_ERRORS;
  glGetProgramiv(ProgramObject, GL_LINK_STATUS, &linked);
  printProgramInfoLog(ProgramObject);
  if (!linked)
    return 0;
  return ProgramObject;
}

GLuint ShaderManager::compile(const char *shaderSource, GLuint shaderType) {
  GLint compiled;
  GLuint shaderObject = glCreateShader(shaderType);

  glShaderSource(shaderObject, 1, &shaderSource, nullptr);

  glCompileShader(shaderObject);
  CHECK_GL_ERRORS;
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    std::cerr << shaderSource << std::endl;
    printShaderInfoLog(shaderObject);
    std::cerr << "failed to compile shader\n";
  }

  return shaderObject;
}

GLuint ShaderManager::createProgram(const std::vector<GLuint> &objects) {
  GLuint programObject = glCreateProgram();
  for (unsigned int object : objects)
    if (object)
      glAttachShader(programObject, object);
  glLinkProgram(programObject);
  CHECK_GL_ERRORS;
  GLint linked;
  glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
  printProgramInfoLog(programObject);
  if (!linked)
    return 0;
  return programObject;
}

} // circe namespace

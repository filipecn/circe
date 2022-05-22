/// Copyright (c) 2022, FilipeCN.
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
///\file program_manager.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-05-19
///
///\brief

#ifndef CIRCE_CIRCE_GL_GRAPHICS_PROGRAM_MANAGER_H
#define CIRCE_CIRCE_GL_GRAPHICS_PROGRAM_MANAGER_H

#include <circe/gl/graphics/shader.h>
#include <hermes/common/result.h>

namespace circe::gl {

///
struct ProgramHandle {
  size_t program_id;
};

// *********************************************************************************************************************
//                                                                                                     ProgramManager
// *********************************************************************************************************************
/// Singleton to manage shader program instances
class ProgramManager {
public:
  // *******************************************************************************************************************
  //                                                                                                   STATIC METHODS
  // *******************************************************************************************************************
  /// \brief Gets singleton instance
  /// \return
  static ProgramManager &instance();
  ///
  static void cleanup();
  //                                                                                                         creation
  /// \brief Sets a directory for shader file searching
  /// \param shader_directory
  static void setShaderSearchPath(const hermes::Path &shader_directory);
  /// \brief Compile a new shader from name
  /// \note program_name must be the same name of shader files in the search directory
  /// \param program_name
  /// \return
  static hermes::Result<ProgramHandle> push(const std::string &program_name);
  /// \brief Compile a new shader from files in directory
  /// \note program_name must be the same name of shader files in the directory
  /// \param shader_directory
  /// \param program_name
  /// \return
  static hermes::Result<ProgramHandle> push(const hermes::Path &shader_directory, const std::string &program_name);
  /// \brief Compile a new shader from circe::gl::Program constructor arguments
  /// \tparam Args
  /// \param args
  /// \return
  template<class ...Args>
  static hermes::Result<ProgramHandle> pushAndLink(Args ...args) {
    Program new_program(std::forward<Args...>(args...));
    if (!new_program.good()) {
      HERMES_LOG_WARNING("Failed to compile/link shader program: {}", new_program.err);
      return hermes::Result<ProgramHandle>({HeResult::ERROR});
    }
    size_t program_id = instance_.programs_.size();
    if (!instance_.free_program_indices_.empty()) {
      instance_.programs_[program_id] = std::move(new_program);
      instance_.free_program_indices_.pop();
    } else
      instance_.programs_.emplace_back(new_program);
    static size_t program_name_id = 0;
    auto program_name = hermes::Str::concat("program", program_name_id++);
    instance_.program_name_to_program_index_map_[program_name] = program_id;
    instance_.program_index_to_program_name_map_[program_id] = program_name;
    return hermes::Result<ProgramHandle>({program_id});
  }
  //                                                                                                           access
  /// \brief Gets program index from program name
  /// \param program_name
  /// \return
  static hermes::Result<size_t> programIndex(const std::string &program_name);
  /// \brief Gets program name from program index
  /// \param program_index
  /// \return
  static hermes::Result<std::string> programName(size_t program_index);
  ///
  /// \param handle
  /// \return
  static hermes::Result<Program*> program(const ProgramHandle& handle);
  /// \brief Gets program reference from name
  /// \param program_name
  /// \return
  static hermes::Result<Program *> program(const std::string &program_name);
  /// \brief Get program reference from index
  /// \param program_name
  /// \return
  static hermes::Result<Program *> program(size_t program_index);
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  ~ProgramManager();
  //                                                                                                       assignment
  ProgramManager(const ProgramManager &) = delete;
  ProgramManager(ProgramManager &&) = delete;
  // *******************************************************************************************************************
  //                                                                                                        OPERATORS
  // *******************************************************************************************************************
  ProgramManager &operator=(const ProgramManager &) = delete;
  ProgramManager &operator=(ProgramManager &&) = delete;

private:
  ProgramManager() noexcept;

  static ProgramManager instance_;

  hermes::Path shader_files_search_path_;
  std::stack<size_t> free_program_indices_;
  std::unordered_map<std::string, size_t> program_name_to_program_index_map_;
  std::unordered_map<size_t, std::string> program_index_to_program_name_map_;
  std::vector<Program> programs_;
};

}

#endif //CIRCE_CIRCE_GL_GRAPHICS_PROGRAM_MANAGER_H

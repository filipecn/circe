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
///\file program_manager.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-05-19
///
///\brief

#include <circe/gl/graphics/program_manager.h>

namespace circe::gl {

ProgramManager ProgramManager::instance_;

ProgramManager &ProgramManager::instance() {
  return instance_;
}

ProgramManager::~ProgramManager() = default;

ProgramManager::ProgramManager() noexcept {}

void ProgramManager::setShaderSearchPath(const hermes::Path &shader_directory) {
  instance_.shader_files_search_path_ = shader_directory;
}

hermes::Result<ProgramHandle> ProgramManager::push(const std::string &program_name) {
  return ProgramManager::push(instance_.shader_files_search_path_, program_name);
}

hermes::Result<ProgramHandle> ProgramManager::push(const hermes::Path &shader_directory,
                                                     const std::string &program_name) {
  Program new_program;
  if (!new_program.link(shader_directory, program_name)) {
    HERMES_LOG_WARNING("Failed to compile/link shader program: {}", new_program.err);
    return hermes::Result<ProgramHandle>({HeResult::ERROR});
  }
  size_t program_id = instance_.programs_.size();
  if (!instance_.free_program_indices_.empty()) {
    instance_.programs_[program_id] = std::move(new_program);
    instance_.free_program_indices_.pop();
  } else
    instance_.programs_.emplace_back(new_program);
  instance_.program_name_to_program_index_map_[program_name] = program_id;
  instance_.program_index_to_program_name_map_[program_id] = program_name;
  return hermes::Result<ProgramHandle>({program_id});
}

hermes::Result<size_t> ProgramManager::programIndex(const std::string &program_name) {
  auto it = instance_.program_name_to_program_index_map_.find(program_name);
  if (it == instance_.program_name_to_program_index_map_.end())
    return hermes::Result<size_t>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<size_t>(it->second);
}

hermes::Result<std::string> ProgramManager::programName(size_t program_index) {
  auto it = instance_.program_index_to_program_name_map_.find(program_index);
  if (it == instance_.program_index_to_program_name_map_.end())
    return hermes::Result<std::string>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<std::string>(it->second);
}

hermes::Result<Program *> ProgramManager::program(const std::string &program_name) {
  auto it = instance_.program_name_to_program_index_map_.find(program_name);
  if (it == instance_.program_name_to_program_index_map_.end())
    return hermes::Result<Program *>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<Program *>(&instance_.programs_[it->second]);
}

hermes::Result<Program *> ProgramManager::program(size_t program_index) {
  if (program_index >= instance_.programs_.size())
    return hermes::Result<Program *>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<Program *>(&instance_.programs_[program_index]);
}

hermes::Result<Program *> ProgramManager::program(const ProgramHandle &handle) {
  if (handle.program_id >= instance_.programs_.size())
    return hermes::Result<Program *>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<Program *>(&instance_.programs_[handle.program_id]);
}

void ProgramManager::cleanup() {
  instance_.programs_.clear();
}

}

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
///\file scene_resource_manager.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-05-21
///
///\brief

#ifndef CIRCE_CIRCE_GL_SCENE_SCENE_RESOURCE_MANAGER_H
#define CIRCE_CIRCE_GL_SCENE_SCENE_RESOURCE_MANAGER_H

#include <circe/gl/scene/scene_model.h>
#include <hermes/common/result.h>

namespace circe::gl {

struct SceneModelHandle {
  size_t resource_index;
};

// *********************************************************************************************************************
//                                                                                                  SceneResourceManager
// *********************************************************************************************************************
/// \brief Singleton class for scene buffer management
class SceneResourceManager {
public:
  // *******************************************************************************************************************
  //                                                                                                   STATIC METHODS
  // *******************************************************************************************************************
  //                                                                                                         instance
  /// \brief Gets singleton instance
  /// \return
  static SceneResourceManager &instance();
  ///
  static void cleanup();
  //                                                                                                      scene model
  ///
  /// \param path
  /// \param options
  /// \return
  static hermes::Result<SceneModelHandle> pushModel(const hermes::Path &path,
                                                    shape_options options = shape_options::none);
  ///
  /// \param model
  /// \return
  static hermes::Result<SceneModelHandle> pushModel(const circe::Model &model);
  ///
  /// \param model
  /// \return
  static hermes::Result<SceneModelHandle> pushModel(circe::Model &&model);
  /// \tparam Args
  /// \param args
  /// \return
  template<class ...Args>
  static hermes::Result<SceneModelHandle> pushModel(Args ...args) {
    return hermes::Result<SceneModelHandle>({HeResult::NOT_IMPLEMENTED});
  }
  //                                                                                               scene model access
  ///
  /// \param handle
  /// \return
  static hermes::Result<SceneModel *> model(const SceneModelHandle &handle);
  ///
  /// \param model_resource_index
  /// \return
  static hermes::Result<SceneModelHandle> modelHandle(size_t model_resource_index);
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  ~SceneResourceManager();
  SceneResourceManager(const SceneResourceManager &) = delete;
  SceneResourceManager(SceneResourceManager &&) = delete;
  //                                                                                                       assignment
  // *******************************************************************************************************************
  //                                                                                                        OPERATORS
  // *******************************************************************************************************************
  //                                                                                                       assignment
  SceneResourceManager &operator=(const SceneResourceManager &) = delete;
  SceneResourceManager &operator=(SceneResourceManager &&) = delete;

private:
  SceneResourceManager() noexcept;

  static SceneResourceManager instance_;

  std::vector<SceneModel> models_;
};

}

#endif //CIRCE_CIRCE_GL_SCENE_SCENE_RESOURCE_MANAGER_H

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
///\file scene_resource_manager.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-05-21
///
///\brief

#include <circe/gl/scene/scene_resource_manager.h>

namespace circe::gl {

SceneResourceManager SceneResourceManager::instance_;

SceneResourceManager &SceneResourceManager::instance() {
  return instance_;
}

SceneResourceManager::SceneResourceManager() noexcept {}

hermes::Result<SceneModelHandle> SceneResourceManager::pushModel(const hermes::Path &path, shape_options options) {
  auto model = Model::fromFile(path, options);
  size_t model_index = instance_.models_.size();
  instance_.models_.emplace_back(model);
  return hermes::Result<SceneModelHandle>({model_index});
}

hermes::Result<SceneModelHandle> SceneResourceManager::pushModel(const Model &model) {
  // TODO handle errors
  size_t model_index = instance_.models_.size();
  instance_.models_.emplace_back(model);
  return hermes::Result<SceneModelHandle>({model_index});
}

hermes::Result<SceneModelHandle> SceneResourceManager::pushModel(Model &&model) {
  // TODO handle errors
  size_t model_index = instance_.models_.size();
  instance_.models_.emplace_back(std::move(model));
  return hermes::Result<SceneModelHandle>({model_index});
}

hermes::Result<SceneModel *> SceneResourceManager::model(const SceneModelHandle &handle) {
  if (handle.resource_index >= instance_.models_.size())
    return hermes::Result<SceneModel *>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<SceneModel *>(&instance_.models_[handle.resource_index]);
}

hermes::Result<SceneModelHandle> SceneResourceManager::modelHandle(size_t model_resource_index) {
  if (model_resource_index >= instance_.models_.size())
    return hermes::Result<SceneModelHandle>({HeResult::OUT_OF_BOUNDS});
  return hermes::Result<SceneModelHandle>({model_resource_index});
}

void SceneResourceManager::cleanup() {
  instance_.models_.clear();
}

SceneResourceManager::~SceneResourceManager() = default;

}
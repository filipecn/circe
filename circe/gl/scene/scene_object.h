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

#ifndef CIRCE_SCENE_SCENE_OBJECT_H
#define CIRCE_SCENE_SCENE_OBJECT_H

#include <ponos/ponos.h>

#include <circe/gl/graphics/shader.h>
#include <circe/gl/io/buffer.h>
#include <circe/io/utils.h>
#include <circe/gl/scene/scene_mesh.h>
#include <circe/gl/ui/interactive_object_interface.h>

#include <utility>

namespace circe::gl {

/// The Scene Object Interface represents an drawable object that can be
/// intersected by a ray and drawn on the screen. It holds a transform that
/// is applied before rendering.
class SceneObject : public InteractiveObjectInterface {
public:
  SceneObject() : visible(true) {}
  virtual ~SceneObject() {}
  /// render method
  /// \param c cur active camera
  /// \param t object transform
  virtual void draw(const CameraInterface *c, ponos::Transform t) = 0;
  /// query
  /// \param r **[in]** ray
  /// \param t **[out]** receives the parametric value of the intersection
  /// \return **true** if intersection is found
  virtual bool intersect(const ponos::Ray3 &r, float *t = nullptr) {
    UNUSED_VARIABLE(t);
    UNUSED_VARIABLE(r);
    return false;
  }

  void updateTransform() override {
    transform = this->trackball.tb.getTransform() * transform;
  }

  ponos::Transform transform;
  bool visible;
};

class SceneMeshObject : public SceneObject {
public:
  SceneMeshObject() {}
  SceneMeshObject(const std::string &filename) {
    ponos::RawMeshSPtr rawMesh(new ponos::RawMesh());
    loadOBJ(filename, rawMesh.get());
    rawMesh->computeBBox();
    rawMesh->splitIndexData();
    rawMesh->buildInterleavedData();
    mesh_ = createSceneMeshPtr(rawMesh.get());
  }
  SceneMeshObject(const std::string &filename, ShaderProgramPtr s)
      : SceneMeshObject(filename) {
    shader_ = s;
  }
  SceneMeshObject(const ponos::RawMesh *m, ShaderProgramPtr s) {
    mesh_ = createSceneMeshPtr(m);
    shader_ = s;
  }
  SceneMeshObject(
      const ponos::RawMesh *m,
      std::function<void(ShaderProgram *, const CameraInterface *,
                         ponos::Transform)>
      f =
      [](ShaderProgram *s, const CameraInterface *camera,
         ponos::Transform t) {
        UNUSED_VARIABLE(s);
        UNUSED_VARIABLE(camera);
        UNUSED_VARIABLE(t);
      },
      ShaderProgramPtr s = nullptr) {
    this->visible = true;
    shader_ = s;
    drawCallback = f;
    mesh_ = createSceneMeshPtr(m);
  }
  virtual ~SceneMeshObject() {}
  void draw(const CameraInterface *camera, ponos::Transform t) override {
    if (!visible)
      return;
    mesh_->bind();
    if (shader_) {
      mesh_->vertexBuffer()->locateAttributes(*shader_.get());
      shader_->begin();
    } else {
      glEnableVertexAttribArray(0);
    }
    if (drawCallback)
      drawCallback(shader_.get(), camera, transform * t);
    glDrawElements(mesh_->indexBuffer()->bufferDescriptor.element_type,
                   mesh_->indexBuffer()->bufferDescriptor.element_count *
                       mesh_->indexBuffer()->bufferDescriptor.element_size,
                   GL_UNSIGNED_INT, 0);
    mesh_->unbind();
    if (shader_)
      shader_->end();
  }
  void setShader(ShaderProgramPtr shader) { shader_ = shader; }
  ShaderProgramPtr shader() { return shader_; }
  SceneMeshSPtr mesh() { return mesh_; }

  std::function<void(ShaderProgram *, const CameraInterface *,
                     ponos::Transform)>
      drawCallback;

protected:
  SceneMeshSPtr mesh_;
  ShaderProgramPtr shader_;
};

/// \brief
class SceneDynamicMeshObject : public SceneObject {
public:
  SceneDynamicMeshObject() {}
  SceneDynamicMesh &mesh() { return mesh_; }
  void draw(const CameraInterface *camera, ponos::Transform t) override {
    if (!visible)
      return;
    mesh_.bind();
    if (shader_) {
      mesh_.vertexBuffer()->locateAttributes(*shader_.get());
      shader_->begin();
    } else
      glEnableVertexAttribArray(0);
    if (draw_callback)
      draw_callback(shader_.get(), camera, transform * t);
    glDrawElements(mesh_.indexBuffer()->bufferDescriptor.element_type,
                   mesh_.indexBuffer()->bufferDescriptor.element_count *
                       mesh_.indexBuffer()->bufferDescriptor.element_size,
                   GL_UNSIGNED_INT, 0);
    mesh_.unbind();
    if (shader_)
      shader_->end();
    CHECK_GL_ERRORS;
  }
  void setShader(ShaderProgramPtr shader) { shader_ = shader; }

  std::function<void(ShaderProgram *, const CameraInterface *,
                     ponos::Transform)>
      draw_callback;

protected:
  SceneDynamicMesh mesh_;
  ShaderProgramPtr shader_;
};

typedef std::shared_ptr<SceneObject> SceneObjectSPtr;
typedef std::shared_ptr<SceneMeshObject> SceneMeshObjectSPtr;

template<typename... TArg>
SceneMeshObjectSPtr createSceneMeshObjectSPtr(TArg &&... Args) {
  return std::make_shared<SceneMeshObject>(std::forward<TArg>(Args)...);
}

} // namespace circe

#endif // CIRCE_SCENE_SCENE_OBJECT_H

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
///\file camera_control.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-03-04
///
///\brief

#ifndef CIRCE_CIRCE_UI_CAMERA_CONTROL_H
#define CIRCE_CIRCE_UI_CAMERA_CONTROL_H

#include <circe/scene/camera_interface.h>

namespace circe {

enum class CameraControlMode {
  NONE,
  PAN,
  ROTATE,
  SCALE,
  Z,
  ZOOM,
  ORBIT,
  FIRST_PERSON,
  CUSTOM
};

// *********************************************************************************************************************
//                                                                                                  CameraControlMode
// *********************************************************************************************************************
/// Defines the behavior of a camera been manipulated.
class CameraControlModeInterface {
public:
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  CameraControlModeInterface() : dragging_(false) {}
  virtual ~CameraControlModeInterface() = default;
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  /// Starts the manipulation (usually triggered by a button press)
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  virtual void start(const CameraInterface &camera, hermes::point2 p);
  /// Updates track mode state (usually after a mouse movement or scroll)
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  /// \param d scroll delta
  virtual void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) = 0;
  /// Stops the manipulation (usually after a button release)
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  virtual void stop(CameraInterface &camera, hermes::point2 p);
  /// \return true if active
  [[nodiscard]] bool isActive() const { return dragging_; }

protected:
  bool dragging_;
  hermes::point2 start_;
};

/// Applies scale
class CameraControlScaleMode : public CameraControlModeInterface {
public:
  CameraControlScaleMode();
  ~CameraControlScaleMode() override;
  void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) override;
};

/// Applies translation
class CameraControlPanMode : public CameraControlModeInterface {
public:
  CameraControlPanMode();
  ~CameraControlPanMode() override;
  void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) override;
};

/// Applies translation in view direction
class CameraControlZMode : public CameraControlModeInterface {
public:
  CameraControlZMode();
  ~CameraControlZMode() override;
  void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) override;
};

/// Applies orbit rotation around up vector
class CameraControlOrbitMode : public CameraControlModeInterface {
public:
  CameraControlOrbitMode();
  ~CameraControlOrbitMode() override;
  void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) override;
};

/// Moves position in view direction
class CameraControlZoomMode : public CameraControlModeInterface {
public:
  CameraControlZoomMode();
  ~CameraControlZoomMode() override;
  void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) override;
};

/// Moves as first person view
class CameraControlFirstPersonMode : public CameraControlModeInterface {
public:
  CameraControlFirstPersonMode();
  ~CameraControlFirstPersonMode() override;
  void update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) override;
};

}

#endif //CIRCE_CIRCE_UI_CAMERA_CONTROL_H

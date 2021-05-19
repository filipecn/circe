/// Copyright (c) 2021, FilipeCN.
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
///\file irradiance_map.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-05-16
///
///\brief

#ifndef CIRCE_CIRCE_GL_GRAPHICS_IBL_H
#define CIRCE_CIRCE_GL_GRAPHICS_IBL_H

#include <circe/gl/texture/texture.h>

namespace circe::gl {

class IBL {
public:
  // ***********************************************************************
  //                           STATIC METHODS
  // ***********************************************************************
  ///
  /// \param texture
  /// \param resolution
  /// \return
  static Texture irradianceMap(const Texture &texture, const ponos::size2 &resolution);
  ///
  /// \param texture
  /// \param resolution
  /// \return
  static Texture preFilteredEnvironmentMap(const Texture &texture, const ponos::size2 &resolution);
  ///
  /// \param resolution
  /// \return
  static Texture brdfIntegrationMap(const ponos::size2 &resolution);
  ///
  /// \param path
  /// \return
  static IBL fromFile(const ponos::Path &path);
};

}

#endif //CIRCE_CIRCE_GL_GRAPHICS_IBL_H
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
///\file options.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-05-13
///
///\brief

#ifndef CIRCE_CIRCE_UI_OPTIONS_H
#define CIRCE_CIRCE_UI_OPTIONS_H

#include <circe/common/bitmask_operators.h>

namespace circe {

/// App configuration
enum class app_options {
  none = 0x00, //!< default setup
  no_viewport = 0x04, //!< starts with no viewport
  no_input = 0x08, //!< viewports will not process input
};
CIRCE_ENABLE_BITMASK_OPERATORS(app_options);

/// App configuration
enum class viewport_options {
  none = 0x00, //!< default setup
  orthographic = 0x01, //!< starts with an orthographic camera
  perspective = 0x02, //!< starts with a perspective camera
  no_input = 0x08, //!< viewports will not process input
};
CIRCE_ENABLE_BITMASK_OPERATORS(viewport_options);

} // circe namespace

#endif //CIRCE_CIRCE_UI_OPTIONS_H

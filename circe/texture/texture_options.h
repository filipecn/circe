//
// Created by filipecn on 13/05/2021.
//


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
///\file texture_options.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-05-13
///
///\brief

#ifndef CIRCE_CIRCE_TEXTURE_TEXTURE_OPTIONS_H
#define CIRCE_CIRCE_TEXTURE_TEXTURE_OPTIONS_H

#include <circe/common/bitmask_operators.h>

namespace circe {

enum class texture_options {
  none = 0x00,
  hdr = 0x01,
  equirectangular = 0x02, //!< texture represents a equirectangular map
  cubemap = 0x04, //!< texture is cubemap
};

CIRCE_ENABLE_BITMASK_OPERATORS(texture_options);

}

#endif //CIRCE_CIRCE_TEXTURE_TEXTURE_OPTIONS_H

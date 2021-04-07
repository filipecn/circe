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
///\file texture_atlas.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-04-07
///
///\brief

#ifndef CIRCE_CIRCE_GL_TEXTURE_TEXTURE_ATLAS_H
#define CIRCE_CIRCE_GL_TEXTURE_TEXTURE_ATLAS_H

#include <circe/gl/texture/texture.h>

namespace circe::gl {

/// A Texture Atlas can be seen as a big texture that arranges smaller textures
/// into it. It can be very useful to store animation sprites, shadow maps and
/// other resources that are usually accessed in the same draw loop.
class TextureAtlas : public Texture {
public:
  struct Region {
    ponos::index2 offset;
    ponos::size2 size;
  };
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  TextureAtlas();
  ~TextureAtlas() override;
  // ***********************************************************************
  //                              REGIONS
  // ***********************************************************************
  size_t push(const Region &region);
  const Region &operator[](int i) const;
  Region &operator[](int i);
  // ***********************************************************************
  //                              METRICS
  // ***********************************************************************
  /// \return total atlas size in texels
  ponos::size2 size_in_texels();
private:
  std::vector<Region> regions_;
  ponos::size2 size_in_texels_;

};

}

#endif //CIRCE_CIRCE_GL_TEXTURE_TEXTURE_ATLAS_H

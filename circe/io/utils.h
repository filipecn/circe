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

#ifndef CIRCE_SCENE_IO_UTILS_H
#define CIRCE_SCENE_IO_UTILS_H

#include <ponos/ponos.h>

#include <string>

namespace circe {

/** \brief load OBJ
 * \param filename **[in]** file path
 * \param mesh **[out]** output
 * Loads an OBJ file and stores its contents in **mesh**
 */
void loadOBJ(const std::string &filename, ponos::RawMesh *mesh);
/** \brief load OBJ
 * \param filename **[in]** file path
 * \param mesh **[out]** output
 * Loads an PLY file and stores its contents in **mesh**
 */
void loadPLY(const std::string &filename, ponos::RawMesh *mesh);

} // circe namespace

#endif // CIRCE_SCENE_IO_UTILS_H

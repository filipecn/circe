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

#include <circe/gl/scene/wireframe_mesh.h>

namespace circe::gl {

WireframeMesh::WireframeMesh(const std::string &filename)
    : SceneMeshObject(filename) {}

WireframeMesh::WireframeMesh(ponos::RawMesh *m, const ponos::Transform &t) {
  UNUSED_VARIABLE(m);
  UNUSED_VARIABLE(t);
  // rawMesh = m;
  // setupVertexBuffer();
  // setupIndexBuffer();
  transform = t;
}

void WireframeMesh::draw(const CameraInterface *camera,
                         ponos::Transform transform) {
  UNUSED_VARIABLE(camera);
  // glPushMatrix();
  // vb->bind();
  // ib->bind();
  float pm[16];
  transform.matrix().column_major(pm);
  glMultMatrixf(pm);
  glColor4f(0, 0, 0, 0.1);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
  // glDrawElements(GL_LINES, ib->bufferDescriptor.elementCount,
  // GL_UNSIGNED_INT,
  //  0);
  glPopMatrix();
}

void WireframeMesh::setupIndexBuffer() {
  // BufferDescriptor indexDescriptor =
  // create_index_buffer_descriptor(1, rawMesh->positionsIndices.size(),
  //  ponos::GeometricPrimitiveType::LINES);
  // ib.reset(new IndexBuffer(&rawMesh->positionsIndices[0], indexDescriptor));
}

} // namespace circe

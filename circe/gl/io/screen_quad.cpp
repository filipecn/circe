// Created by filipecn on 8/26/18.
/*
 * Copyright (c) 2018 FilipeCN
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

#include "screen_quad.h"
#include "buffer.h"
#include <circe/gl/graphics/shader.h>
#include <ponos/structures/raw_mesh.h>

namespace circe::gl {

ScreenQuad::ScreenQuad() {
  mesh_.meshDescriptor.elementSize = 3;
  mesh_.meshDescriptor.count = 2;
  mesh_.positionDescriptor.elementSize = 2;
  mesh_.positionDescriptor.count = 4;
  mesh_.texcoordDescriptor.elementSize = 2;
  mesh_.texcoordDescriptor.count = 4;
  mesh_.positions = std::vector<float>({-1, -1, 1, -1, 1, 1, -1, 1});
  mesh_.texcoords = std::vector<float>({0, 0, 1, 0, 1, 1, 0, 1});
  mesh_.indices.resize(6);
  mesh_.indices[0].positionIndex = mesh_.indices[0].texcoordIndex = 0;
  mesh_.indices[1].positionIndex = mesh_.indices[1].texcoordIndex = 1;
  mesh_.indices[2].positionIndex = mesh_.indices[2].texcoordIndex = 2;
  mesh_.indices[3].positionIndex = mesh_.indices[3].texcoordIndex = 0;
  mesh_.indices[4].positionIndex = mesh_.indices[4].texcoordIndex = 2;
  mesh_.indices[5].positionIndex = mesh_.indices[5].texcoordIndex = 3;
  mesh_.splitIndexData();
  mesh_.buildInterleavedData();
  const char *fs = "#version 440 core\n"
                   "out vec4 outColor;"
                   "in vec2 texCoord;"
                   "layout (location = 0) uniform sampler2D tex;"
                   "void main(){"
                   " outColor = texture(tex, texCoord);}";
  const char *vs = "#version 440 core\n"
                   "layout (location = 0) in vec2 position;"
                   "layout (location = 1) in vec2 texcoord;"
                   "out vec2 texCoord;"
                   "void main() {"
                   " texCoord = texcoord;"
                   " gl_Position = vec4(position, 0, 1);}";
  const char *gs = nullptr;
  shader.reset(new ShaderProgram(vs, gs, fs));
  shader->addVertexAttribute("position", 0);
  shader->addVertexAttribute("texcoord", 1);
  shader->addUniform("tex", 0);
  {
    glGenVertexArrays(1, &VAO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    BufferDescriptor vd, id;
    create_buffer_description_from_mesh(mesh_, vd, id);
    vb_.reset(new GLVertexBuffer(&mesh_.interleavedData[0], vd));
    ib_.reset(new GLIndexBuffer(&mesh_.positionsIndices[0], id));
    vb_->locateAttributes(*shader.get());
    //    shader->registerVertexAttributes(vb_.get());
    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards we
    // can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // remember: do NOT unbind the EBO while a VAO is active as the bound
    // element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't
    // unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
  }
}

ScreenQuad::~ScreenQuad() = default;

void ScreenQuad::render() {
  glBindVertexArray(VAO);
  shader->begin();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  shader->end();
  glBindVertexArray(0);
}
}
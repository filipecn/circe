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
///\file imgui_utils.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-10
///
///\brief

#ifndef CIRCE_CIRCE_UI_IMGUI_UTILS_H
#define CIRCE_CIRCE_UI_IMGUI_UTILS_H

#include <circe/imgui/ImGuiFileDialog.h>
#include <hermes/common/file_system.h>

namespace circe {

class ImguiFolderDialog {
public:
  struct Result {
    hermes::Path path;
    explicit operator bool() const {
      return valid;
    }
    bool valid{false};
  };
  static Result folder_dialog_button(const std::string &label, const hermes::Path &path = ".");

};

}

#endif //CIRCE_CIRCE_UI_IMGUI_UTILS_H

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
#include <vector>

namespace circe {

///
/// \tparam E
template<typename E>
struct ImGuiRadioButtonSet {
  ImGuiRadioButtonSet() = default;
  ~ImGuiRadioButtonSet() = default;

  const E &currentOption() const { return current_choice_; }

  void push(const std::string &label, E type_value) {
    RadioOption o = {
        .type_id = type_value,
        .label = label
    };
    options_.push_back(o);
  }

  void draw(const std::function<void(E)> &pick_callback = nullptr) {
    for (const auto &option : options_)
      if (ImGui::RadioButton(option.label.c_str(), option.type_id == current_choice_)) {
        current_choice_ = option.type_id;
        if (pick_callback)
          pick_callback(current_choice_);
      }
  }

private:
  struct RadioOption {
    E type_id;
    std::string label;
  };
  std::vector<RadioOption> options_;
  E current_choice_;
};

class ImguiOpenDialog {
public:
  struct Result {
    hermes::Path path;
    explicit operator bool() const {
      return valid;
    }
    bool valid{false};
  };
  static Result folder_dialog_button(const std::string &label, const hermes::Path &path = ".");
  static Result folder_dialog_menu_item(const std::string &label, const hermes::Path &path = ".");
  /// \brief Open file dialog button
  /// \param label button label
  /// \param extensions accepted file extensions list (separated by ',')
  /// \param path root path
  /// \return Result object containing file path
  static Result file_dialog_button(const std::string &label,
                                   const std::string &extensions,
                                   const hermes::Path &path = ".");
  /// \brief Open file dialog menu button
  /// \param label button label
  /// \param extensions accepted file extensions list (separated by ',')
  /// \param path root path
  /// \return Result object containing file path
  static Result file_dialog_menu_item(const std::string &label,
                                      const std::string &extensions,
                                      const hermes::Path &path = ".");

};

}

#endif //CIRCE_CIRCE_UI_IMGUI_UTILS_H

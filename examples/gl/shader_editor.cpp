/// Copyright (c) 2020, FilipeCN.
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
///\file shader_editor.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-26-08
///
///\brief

#include <circe/circe.h>

using namespace circe::gl;

class ShaderEditor : public BaseApp {
public:
  ShaderEditor() : BaseApp(800, 800) {
    auto lang = TextEditor::LanguageDefinition::GLSL();
    vertex_editor.SetLanguageDefinition(lang);
    fragment_editor.SetLanguageDefinition(lang);
    reset();

  }

  void render(circe::CameraInterface *camera) override {
    showControls();
    showEditor(vertex_editor, "Vertex Shader");
    showEditor(fragment_editor, "Fragment Shader");
  }

  void showControls() {
    ImGui::Begin("Constrols");
    std::stringstream ss;
    ss << this->last_FPS_ << "fps" << std::endl;
    ImGui::Text("%s", ss.str().c_str());
    if (ImGui::Button("reset"))
      reset();
    showLoadFile(vertex_editor, "Open Vertex Shader", "OpenFileVSKey");
    showLoadFile(fragment_editor, "Open Fragment Shader", "OpenFileFSKey");
    ImGui::End();
  }

  static void showEditor(TextEditor &editor, const std::string &title) {
    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    auto cpos = editor.GetCursorPosition();
    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                editor.IsOverwrite() ? "Ovr" : "Ins",
                editor.CanUndo() ? "*" : " ",
                editor.GetLanguageDefinition().mName.c_str(), title.c_str());

    editor.Render("TextEditor");
    ImGui::End();
  }

  static void showLoadFile(TextEditor &editor, const std::string &label, const std::string &key) {
    if (ImGui::Button(label.c_str()))
      igfd::ImGuiFileDialog::Instance()->OpenDialog(key, "Choose File", "", ".");
    if (igfd::ImGuiFileDialog::Instance()->FileDialog(key)) {
      if (igfd::ImGuiFileDialog::Instance()->IsOk)
        editor.SetText(ponos::FileSystem::readFile(igfd::ImGuiFileDialog::Instance()->GetFilePathName()));
      igfd::ImGuiFileDialog::Instance()->CloseDialog(key);
    }
  }

  void reset() {
    auto vs = std::string(SHADERS_PATH) + "/basic.vert";
    auto fs = std::string(SHADERS_PATH) + "/basic.frag";
    vertex_editor.SetText(ponos::FileSystem::readFile(vs));
    fragment_editor.SetText(ponos::FileSystem::readFile(fs));
  }

  TextEditor vertex_editor, fragment_editor;
};

int main() {
  ShaderEditor app;
  return app.run();
}
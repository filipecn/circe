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
///\file imgui_logger.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-01-09
///
///\brief

#include <circe/ui/imgui_logger.h>
#include <circe/imgui/imgui.h>

namespace circe {

HLogger::HLogger() {
  setMaxLogCount(10);
  // config hermes log
  hermes::Log::abbreviation_size = 5;
  hermes::Log::removeOptions(hermes::logging_options::use_colors
                                 | hermes::logging_options::full_path_location);
  // register callback
  hermes::Log::log_callback = [&](const hermes::Str &s, hermes::logging_options options) {
    logMessage(s, options);
  };
}

HLogger::~HLogger() {

}

void HLogger::render() {
  ImGui::Combo("##log_combo", &current_mode_, "All\0Info\0Warn\0Error\0Critical\0");

  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 60);

  if (enabled_) {
    if (ImGui::Button("sstop"))
      enabled_ = false;
  } else if (ImGui::Button("rresume"))
    enabled_ = true;

  ImGui::BeginChild("Log", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);

  size_t current_line = next_cache_index_ >= max_cache_size_ ? 0 : next_cache_index_;
  do {
    if (current_line >= logged_messages_count)
      break;
    auto options = cache_[current_line].options;
    const auto &color = colorFrom(cache_[current_line].options);
    const char *message = cache_[current_line].message.c_str();
    current_line = (current_line + 1) % max_cache_size_;
    // filter
    if (current_mode_ == 1 && !HERMES_MASK_BIT(options, hermes::logging_options::info))
      continue;
    if (current_mode_ == 2 && !HERMES_MASK_BIT(options, hermes::logging_options::warn))
      continue;
    if (current_mode_ == 3 && !HERMES_MASK_BIT(options, hermes::logging_options::error))
      continue;
    if (current_mode_ == 4 && !HERMES_MASK_BIT(options, hermes::logging_options::critical))
      continue;

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
    ImGui::Text("%s", message);
    ImGui::PopStyleColor(1);
  } while (current_line != next_cache_index_);

  ImGui::EndChild();
}

void HLogger::setMaxLogCount(size_t max_log_count) {
  max_cache_size_ = max_log_count;
  cache_.resize(max_log_count);
}

void HLogger::logMessage(const hermes::Str &m, hermes::logging_options options) {
  if (!enabled_)
    return;
  if (cache_.empty())
    return;
  logged_messages_count++;
  if (next_cache_index_ == cache_.size()) {
    if (cache_.size() < max_cache_size_) {
      cache_.push_back({m, options});
    } else
      next_cache_index_ = 0;
  }
  cache_[next_cache_index_].message = m;
  cache_[next_cache_index_].options = options;
  next_cache_index_ = (next_cache_index_ + 1) % max_cache_size_;
}

void HLogger::enable() {
  enabled_ = true;
}

void HLogger::disable() {
  enabled_ = false;
}

const circe::Color &HLogger::colorFrom(const hermes::logging_options &options) const {
  if (HERMES_MASK_BIT(options, hermes::logging_options::info))
    return info_color;
  if (HERMES_MASK_BIT(options, hermes::logging_options::warn))
    return warn_color;
  if (HERMES_MASK_BIT(options, hermes::logging_options::error))
    return error_color;
  if (HERMES_MASK_BIT(options, hermes::logging_options::critical))
    return critical_color;
  return info_color;
}

}

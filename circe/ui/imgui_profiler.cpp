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
///\file imgui_profiler.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-01-05
///
///\brief

#include <circe/imgui/imgui.h>
#include <circe/ui/imgui_profiler.h>
#include <hermes/common/debug.h>

namespace circe {

HProfiler::HProfiler() {

}

HProfiler::~HProfiler() {

}

static const char *res_names = "tnums";

f64 convertRes(f64 time, HProfiler::Resolution a, HProfiler::Resolution b) {
  int res_diff = static_cast<int>(a) - static_cast<int>(b);
  if (res_diff == 0)
    return time;
  if (res_diff < 0) {
    for (int i = static_cast<int>(a); i < static_cast<int>(b); ++i)
      time /= 1000.f;
    return time;
  }
  for (int i = static_cast<int>(b); i < static_cast<int>(a); ++i)
    time *= 1000.f;
  return time;
}

f64 ticks2res(u64 ticks, HProfiler::Resolution resolution) {
  int r = static_cast<int>(resolution);
  f64 den = 1;
  f64 res = ticks;
  for (int i = 1; i < r; ++i) {
    den *= 1000;
    res = static_cast<f64>(ticks) / den;
  }
  return res;
}

hermes::Str prettyTicks(u64 ticks, f64 *value = nullptr, HProfiler::Resolution *resolution = nullptr) {
  u64 t = hermes::profiler::ticks2ns(ticks);
  if (value)
    *value = t;
  if (resolution)
    *resolution = HProfiler::Resolution::NANOSECONDS;
  if (t < 100)
    return hermes::Str() << t << " " << res_names[static_cast<int>(HProfiler::Resolution::NANOSECONDS)] << "s";
  if (resolution)
    *resolution = HProfiler::Resolution::MICROSECONDS;
  f64 f = static_cast<f64>(t) / 1000.;
  if (value)
    *value = f;
  if (f < 100)
    return hermes::Str() << f << " " << res_names[static_cast<int>(HProfiler::Resolution::MICROSECONDS)] << "s";
  if (resolution)
    *resolution = HProfiler::Resolution::MILLISECONDS;
  f /= 1000.;
  if (value)
    *value = f;
  if (f < 100)
    return hermes::Str() << f << " " << res_names[static_cast<int>(HProfiler::Resolution::MILLISECONDS)] << "s";
  if (resolution)
    *resolution = HProfiler::Resolution::SECONDS;
  f /= 1000.;
  if (value)
    *value = f;
  return hermes::Str() << f << " " << res_names[static_cast<int>(HProfiler::Resolution::SECONDS)];
}

/// code inspired in https://github.com/sebh/Dx11Base/blob/master/Application/WinMain.cpp
void HProfiler::render() {

  u64 window_end = current_time ? current_time : hermes::profiler::now();
  u64 window_start = (window_end > window_size_) ? window_end - window_size_ : 0;
  u64 total_duration = window_end - window_start;

  HERMES_ASSERT(window_start)

  const auto &blocks = hermes::profiler::Profiler::blockList();

  ImGui::PushItemWidth(100);
  ImGui::InputInt("##int", &current_window_size_);
  setTimeWindow(current_window_size_, resolution_);
  ImGui::PopItemWidth();
  ImGui::SameLine();

  ImGui::PushItemWidth(50);
  if (ImGui::Combo("##combo", &current_resolution_, "ns\0us\0ms\0s\0")) {
    auto previous_resolution = resolution_;
    if (current_resolution_ == 0)
      resolution_ = circe::HProfiler::Resolution::NANOSECONDS;
    else if (current_resolution_ == 1)
      resolution_ = circe::HProfiler::Resolution::MICROSECONDS;
    else if (current_resolution_ == 2)
      resolution_ = circe::HProfiler::Resolution::MILLISECONDS;
    else
      resolution_ = circe::HProfiler::Resolution::SECONDS;
    current_window_size_ = convertRes(current_window_size_, previous_resolution, resolution_);
    if (!current_window_size_)
      current_window_size_ = 1;
  }
  ImGui::PopItemWidth();
  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 60);

  stop_profiler_ = false;
  resume_profiler_ = false;
  if (hermes::profiler::Profiler::isEnabled()) {
    if (ImGui::Button("stop"))
      stop_profiler_ = true;
  } else if (ImGui::Button("resume"))
    resume_profiler_ = true;


  ImGui::BeginChild("Timer graph", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);

  u64 total_duration_res = ticks2res(total_duration);

  const float max_width = ImGui::GetWindowWidth() - 20;
  const float ms2pixel = max_width / total_duration_res;

  HERMES_ASSERT(window_end >= window_start)

  auto *painter = ImGui::GetWindowDrawList();

  // draw timeline
  int steps = 10;
  float time_step_res = (float) total_duration_res / (float) steps;
  float time_step = (float) total_duration / (float) steps;
  ImVec2 base_pos = ImGui::GetCursorScreenPos();
  for (int i = 0; i <= steps; ++i) {
    float x = (float) i * time_step_res * ms2pixel;

    ImGui::SetCursorScreenPos(ImVec2(base_pos.x + x, base_pos.y));

    auto time_label = prettyTicks(i * time_step);
    auto text_size = ImGui::CalcTextSize(time_label.c_str());

    painter->AddText(
        ImGui::GetFont(),
        ImGui::GetFontSize() * 0.9f,
        ImVec2{base_pos.x + x - text_size.x * 0.2f, base_pos.y},
        hermes::argb_colors::CreamWhite,
        time_label.c_str()
    );
    painter->AddLine(ImVec2(base_pos.x + x, base_pos.y),
                     ImVec2(base_pos.x + x, base_pos.y - 20),
                     hermes::argb_colors::White);
//                     ImColor::HSV(0.04f, 0.7f, 0.7f));
  }

  ImGui::NewLine();

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));

  bool block_found = true;
  for (u32 level = 0; block_found; ++level) {
    block_found = false;
    hermes::profiler::Profiler::iterateBlocks([&](const hermes::profiler::Profiler::Block &block) {
      if (block.level == level) {
        u64 block_start = window_start < block.begin() ? block.begin() - window_start : 0;
        u64 block_end = block.end() ? block.end() : window_end;
        block_end = window_start < block_end ? block_end - window_start : 0;
        u64 block_duration = block_end - block_start;
        // intersect block
        if ((block.end() && block.end() < window_start) || !block_duration)
          return;

        f64 start = circe::ticks2res(block_start, resolution_);
        f64 end = circe::ticks2res(block_end, resolution_);

        const auto &desc = hermes::profiler::Profiler::blockDescriptor(block);
        ImGui::SetCursorPosX(start * ms2pixel);
        ImGui::PushItemWidth(end * ms2pixel);
        ImU32 color = ImColor(hermes::argb_colors::argb2rgba(desc.color));
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        hermes::Str s;
        s.append(desc.name, ' ', prettyTicks(block_end - block_start));
        ImGui::Button(s.c_str(), ImVec2((end - start) * ms2pixel, 0.0f));
        if (ImGui::IsItemHovered())
          ImGui::SetTooltip("%s", s.c_str());
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);

        block_found = true;
      }
    });
    ImGui::NewLine();
  }

  ImGui::PopStyleVar(3);

  ImGui::EndChild();

  ImGui::Text("Start: %s", prettyTicks(window_start - hermes::profiler::Profiler::initTime()).c_str());
  ImGui::SameLine();
  ImGui::Text("End: %s", prettyTicks(window_end - hermes::profiler::Profiler::initTime()).c_str());
}

void HProfiler::update() {
  if (stop_profiler_) {
    HERMES_DISABLE_PROFILER
    if (!current_time)
      current_time = hermes::profiler::now();
  }
  if (resume_profiler_) {
    HERMES_ENABLE_PROFILER
    current_time = 0;
  }
}

void HProfiler::setTimeWindow(u64 window_size, Resolution window_resolution) {
  switch (window_resolution) {
  case HProfiler::Resolution::TICKS:window_size_ = window_size;
    break;
  case HProfiler::Resolution::NANOSECONDS:window_size_ = hermes::profiler::ns2ticks(window_size);
    break;
  case HProfiler::Resolution::MICROSECONDS:window_size_ = hermes::profiler::us2ticks(window_size);
    break;
  case HProfiler::Resolution::MILLISECONDS:window_size_ = hermes::profiler::ms2ticks(window_size);
    break;
  case HProfiler::Resolution::SECONDS:window_size_ = hermes::profiler::s2ticks(window_size);
    break;
  default:HERMES_NOT_IMPLEMENTED
  }
}

u64 HProfiler::ticks2res(u64 ticks) {
  switch (resolution_) {
  case Resolution::TICKS: break;
  case Resolution::NANOSECONDS: return hermes::profiler::ticks2ns(ticks);
  case Resolution::MICROSECONDS: return hermes::profiler::ticks2us(ticks);
  case Resolution::MILLISECONDS: return hermes::profiler::ticks2ms(ticks);
  case Resolution::SECONDS: return hermes::profiler::ticks2s(ticks);
  }
}

}

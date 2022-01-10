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
///\file imgui_profiler.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-01-05
///
///\brief

#ifndef CIRCE_CIRCE_IMGUI_IMGUI_PROFILER_H
#define CIRCE_CIRCE_IMGUI_IMGUI_PROFILER_H

#include <hermes/common/profiler.h>

namespace circe {

// *********************************************************************************************************************
//                                                                                                      HProfiler
// *********************************************************************************************************************
class HProfiler {
public:
  enum class Resolution {
    TICKS = 0,
    NANOSECONDS = 1,
    MICROSECONDS = 2,
    MILLISECONDS = 3,
    SECONDS = 4
  };
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  HProfiler();
  ~HProfiler();
  //                                                                                                       assignment
  // *******************************************************************************************************************
  //                                                                                                        OPERATORS
  // *******************************************************************************************************************
  //                                                                                                       assignment
  //                                                                                                       arithmetic
  //                                                                                                          boolean
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  void render();
  void setTimeWindow(u64 window_size, Resolution window_resolution = Resolution::NANOSECONDS);
  void update();
  // *******************************************************************************************************************
  //                                                                                                    PUBLIC FIELDS
  // *******************************************************************************************************************
private:
  u64 ticks2res(u64 ticks);

  u64 current_time{0};
  u64 window_size_{hermes::profiler::ms2ticks(3)};
  Resolution resolution_{Resolution::MILLISECONDS};
  // aux
  int current_resolution_{2};
  int current_window_size_{3};
  bool stop_profiler_{false};
  bool resume_profiler_{false};
};

}

#endif //CIRCE_CIRCE_IMGUI_IMGUI_PROFILER_H

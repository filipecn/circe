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
///\file imgui_logger.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-01-09
///
///\brief

#ifndef CIRCE_CIRCE_UI_IMGUI_LOGGER_H
#define CIRCE_CIRCE_UI_IMGUI_LOGGER_H

#include <hermes/logging/logging.h>
#include <circe/colors/color.h>

namespace circe {

// *********************************************************************************************************************
//                                                                                                            HLogger
// *********************************************************************************************************************
class HLogger {
public:
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  HLogger();
  ~HLogger();
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  void setMaxLogCount(size_t max_log_count);
  void logMessage(const hermes::Str &m, hermes::logging_options options);
  void render();
  void enable();
  void disable();
  // *******************************************************************************************************************
  //                                                                                                    PUBLIC FIELDS
  // *******************************************************************************************************************

  circe::Color info_color{circe::Color::White()};
  circe::Color warn_color{circe::Color::Yellow()};
  circe::Color error_color{circe::Color::Red()};
  circe::Color critical_color{circe::Color::Red()};

private:
  [[nodiscard]] const circe::Color &colorFrom(const hermes::logging_options &options) const;

  struct LogMessage {
    hermes::Str message;
    hermes::logging_options options;
  };
  int current_mode_{0};
  bool enabled_{true};
  size_t logged_messages_count{0};
  size_t next_cache_index_{0};
  size_t max_cache_size_{0};
  std::vector<LogMessage> cache_;
};

}

#endif //CIRCE_CIRCE_UI_IMGUI_LOGGER_H

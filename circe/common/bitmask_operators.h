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
///\file bitmask_operators.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-14-08
///
///\brief Add support for enum classes that allow bitwise operations
/// Usage:
/// Suppose have an enum class object and want to perform bitwise operations
/// with its values:
/// enum class Permissions {
///    Readable   = 0x4,
///    Writeable  = 0x2,
///    Executable = 0x1
/// };
/// In order to allow such operations as
/// Permissions p = Permissions::Readable | Permissions::Writable;
/// just add the macro call after declaration:
/// enum class Permissions {..};
/// PONOS_ENABLE_BITMASK_OPERATORS(Permissions);

#ifndef CIRCE_COMMON_BITMASK_OPERATORS_H
#define CIRCE_COMMON_BITMASK_OPERATORS_H

#include <type_traits>

namespace circe {

#define CIRCE_ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct CirceEnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
}

template<typename Enum>
struct CirceEnableBitMaskOperators {
  static const bool enable = false;
};

template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator|(Enum lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      static_cast<underlying>(lhs) |
          static_cast<underlying>(rhs)
  );
}

template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator&(Enum lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      static_cast<underlying>(lhs) &
          static_cast<underlying>(rhs)
  );
}

template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator^(Enum lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      static_cast<underlying>(lhs) ^
          static_cast<underlying>(rhs)
  );
}

template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator~(Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      ~static_cast<underlying>(rhs)
  );
}

}

#endif //PONOS_PONOS_PONOS_COMMON_BITMASK_OPERATORS_H

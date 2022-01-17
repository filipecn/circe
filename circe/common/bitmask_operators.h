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
///\brief Support of bitwise operations for compatible enum classes
///
///\ingroup common
///\addtogroup common
/// @{

#ifndef CIRCE_COMMON_BITMASK_OPERATORS_H
#define CIRCE_COMMON_BITMASK_OPERATORS_H

#include <type_traits>

namespace circe {

/// \brief Adds bitwise operation support to a given enum class
///
/// \code{.cpp}
///     // Suppose you have an enum class object
///     enum class Permissions {
///        Readable   = 0x4,
///        Writeable  = 0x2,
///        Executable = 0x1
///     };
///     // and you want to do things like this
///     Permissions p = Permissions::Readable | Permissions::Writable;
/// \endcode
/// Then, just call this macro after you enum class declaration:
/// \code{.cpp}
///     enum class Permissions {..};
///     CIRCE_ENABLE_BITMASK_OPERATORS(Permissions);
/// \endcode
/// \pre The enum class underlying type must support such operations
/// \param x enum class name
#define CIRCE_ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct CirceEnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
}

/// \brief Tests if enum class value is enabled
///
/// - Example:
/// \code{.cpp}
///     enum class Permissions {
///        Readable   = 0x4,
///        Writeable  = 0x2,
///        Executable = 0x1
///     };
///     CIRCE_ENABLE_BITMASK_OPERATORS(Permissions);
///     void function() {
///         Permissions p = Permissions::Readable | Permissions::Writable;
///         // you can check if Permissions::Executable is in p
///         if(CIRCE_MASK_BIT(p, Permissions::Executable)) {}
///     }
/// \endcode
///
/// \param MASK enum class instance object
/// \param BIT set of values to be looked in MASK
#define CIRCE_MASK_BIT(MASK, BIT) (((MASK) & (BIT)) == (BIT))

/// \brief Wrapper struct to add bitwise operations to enum class
/// \tparam Enum
template<typename Enum>
struct CirceEnableBitMaskOperators {
  static const bool enable = false; //!< enable flag
};

/// adds | operation support
/// \tparam Enum
/// \param lhs
/// \param rhs
/// \return
template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator|(Enum lhs, Enum rhs) {
  /// underlying enum data type
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      static_cast<underlying>(lhs) |
          static_cast<underlying>(rhs)
  );
}

/// adds & operation support
/// \tparam Enum
/// \param lhs
/// \param rhs
/// \return
template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator&(Enum lhs, Enum rhs) {
  /// underlying enum data type
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      static_cast<underlying>(lhs) &
          static_cast<underlying>(rhs)
  );
}

/// \brief adds ^ operation support
/// \tparam Enum
/// \param lhs
/// \param rhs
/// \return
template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator^(Enum lhs, Enum rhs) {
  /// underlying enum data type
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      static_cast<underlying>(lhs) ^
          static_cast<underlying>(rhs)
  );
}

/// \brief adds ~ operation support
/// \tparam Enum
/// \param rhs
/// \return
template<typename Enum>
typename std::enable_if<CirceEnableBitMaskOperators<Enum>::enable, Enum>::type
operator~(Enum rhs) {
  /// underlying enum data type
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
      ~static_cast<underlying>(rhs)
  );
}

}

#endif //CIRCE_COMMON_BITMASK_OPERATORS_H

/// @}

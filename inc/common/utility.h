//! @file utility.h
//! @brief Definition of basic types and operations
//! @author Bobrov A.E.
//! @date 26.04.2020
//! @copyright (c) Bobrov A.E.
#pragma once

#include <type_traits>

namespace common
{
template <typename E>
constexpr auto FromEnum(E enumerator) noexcept
{
  return static_cast<std::underlying_type_t<E>>(enumerator);
}
}

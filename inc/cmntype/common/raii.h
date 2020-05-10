/** @file raii.h
 *  @brief generialize template RAII concept
 *  @author Bobrov A.E.
 *  @date 28.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */
#pragma once

#include <memory>

namespace common
{
template <typename T>
class Raii
{
public:
  explicit Raii(T&& obj)
  : obj_{std::forward<T>(obj)}
  {
  }
  Raii(const Raii&) = delete;
  Raii& operator=(const Raii&) = delete;
  Raii(Raii&&) = default;
  Raii& operator=(Raii&&) = default;
  ~Raii() = default;
  T& Get()
  {
    return obj_;
  }
private:
  T obj_;
};

}

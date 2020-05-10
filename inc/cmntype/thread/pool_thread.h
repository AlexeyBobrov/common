/** @file pool_thread.h
 *  @brief The define class PoolThread
 *  @author Bobrov A.E.
 *  @date 04.12.2019
 */
#pragma once

// std
#include <cstddef>
#include <functional>
#include <memory>

namespace common
{
namespace thread
{
/// @class PoolThread
/// @brief
class PoolThread final
{
 public:
  /// @brief functional object
  using Function = std::function<int()>;
  

 public:
  /// @brief ctor
  /// @param count - count of thread
  explicit PoolThread(std::size_t count = 4);
  /// @brief delete copy semantic
  PoolThread(PoolThread&) = delete;
  PoolThread(const PoolThread&) = delete;
  /// @brief move semantics
  PoolThread(PoolThread&&);
  PoolThread& operator=(PoolThread&&);
  /// @brief dtor
  ~PoolThread();
  /// @brief post task
  /// @param func - functional object func()
  /// @return code exit functional object
  void Post(Function func);

 private:
  /// @brief Pimpl idioms
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace thread
}  // namespace common

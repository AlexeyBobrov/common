/**  @file thread_safe.h
 *   @brief The define class ThreadSafe
 *   @author Bobrov A.E.
 *   @date 28.12.2019
 */
#pragma once

// std
#include <thread>

namespace common
{
namespace thread 
{
class ThreadSafe final
{
public:
  enum class DtorAction
  {
    join,
    detach
  };
public:
  ThreadSafe(std::thread&& t, DtorAction action = DtorAction::join)
  : action_(action)
  , thr_(std::move(t))
  {
    
  }
  ThreadSafe(const ThreadSafe&) = delete;
  ThreadSafe& operator=(const ThreadSafe&) = delete;
  ThreadSafe(ThreadSafe&&) = default;
  ThreadSafe& operator=(ThreadSafe&&) = default;
  ~ThreadSafe()
  {
    Join();
  }

  std::thread& Get()
  {
    return thr_;
  }

  void Join()
  {
    if (thr_.joinable())
    {
      if (DtorAction::detach == action_)
      {
        thr_.detach();
      }
      else if (DtorAction::join == action_)
      {
        thr_.join();
      }
    }
  }

private:
  DtorAction action_;
  std::thread thr_;
};
}
}

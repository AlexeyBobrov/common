/// @file test_thread.cpp
/// @brief Implementation of testing the thread
/// @date 26.02.2020
/// @author Bobrov A.E.
/// @copyright Copyright (c) Bobrov A.E.

#include <atomic>

#include <gtest/gtest.h>

#include <cmntype/thread/pool_thread.h>
#include <cmntype/thread/thread_safe.h>

TEST(TestThread, PoolTheadPostTask)
{
  std::atomic<bool> flag {false};
  {
    common::thread::PoolThread pool( 4 );
    common::thread::PoolThread pool1(std::move(pool));
    auto task = [&flag]
    {
      flag = true;
      return 0;
    };

    pool1.Post(task);
  }
  ASSERT_TRUE(flag);
}

TEST(TestThread, PoolThreadMoveSemantics)
{
  std::atomic<bool> flag{false};
  {
    common::thread::PoolThread pool(2);
    common::thread::PoolThread pool1;
    pool1 = std::move(pool);
    auto task = [&flag]
    {
      flag = true;
      return 0;
    };
    pool1.Post(task);
  }
  ASSERT_TRUE(flag);
}

TEST(TestThread, ThreadDetachTest)
{
  using namespace common::thread;
  
  std::thread thr([]
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      });

  ThreadSafe t(std::move(thr), ThreadSafe::DtorAction::detach);
  t.Join();
  ASSERT_FALSE(t.Get().joinable());
}

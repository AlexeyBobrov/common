/** @file pool_thread.cpp
 *  @brief The implementation of the class PoolThread
 *  @author Bobrov A.E.
 *  @date 06.12.2019
 */
// std
#include <thread>
#include <chrono>
#include <functional>

// boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// this
#include <cmntype/thread/pool_thread.h>

namespace common
{
namespace thread
{

PoolThread::PoolThread(PoolThread&&) = default;
PoolThread& PoolThread::operator=(PoolThread&&) = default;
PoolThread::~PoolThread() = default;

class PoolThread::Impl final
{
public:
  explicit Impl(std::size_t count)
  : pool_(count)
  {

  }
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(Impl&&) = default;
  Impl& operator=(Impl&&) = default;

  void Post(Function task)
  {
    boost::asio::post(pool_, task);
  }

  ~Impl()
  {
    pool_.join();
  }

private:
  boost::asio::thread_pool pool_;
};

PoolThread::PoolThread(std::size_t count)
: impl_(std::make_unique<Impl>(count))
{

}

void PoolThread::Post(Function task)
{
  impl_->Post(task);
}
}
}

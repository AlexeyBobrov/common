/** @file stopwatch.h
 *  @brief The define stopwatch
 *  @author Bobrov A.E.
 *  @date 31.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */
#pragma once

#include <chrono>

namespace common
{
class Stopwatch final
{
public:
  using Clock = std::chrono::high_resolution_clock;
  using Duration = double;
public:
  Stopwatch()
    : start_{Clock::now()}
  {
     
  }
  
  Duration Get() const
  {
    auto finish = Clock::now();
    std::chrono::duration<double> elapsed = finish - start_;
    return elapsed.count();
  }
  
private:
  Clock::time_point start_;
};
}

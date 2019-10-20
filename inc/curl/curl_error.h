/** @file curl_error.h
 *  @brief The define curl error
 *  @author Bobrov A.E.
 *  @date 18.10.2019
 */
#pragma once

#include <cstddef>

#include <error/error.h>

namespace common
{
namespace curl
{
class CurlError : public error::Error
{
 public:
  template <typename Value>
  CurlError(std::uint32_t opt, Value value, const std::string& fileName, const std::string& funcName, std::uint32_t line);
};
}  // namespace curl
}  // namespace common

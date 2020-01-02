/** @file curl_error.h
 *  @brief The define curl error
 *  @author Bobrov A.E.
 *  @date 18.10.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */
#pragma once

// std
#include <cstddef>
#include <string_view>

// boost
#include <boost/format.hpp>

// curl
#include <curl/curl.h>

// error
#include <error/error.h>

namespace common
{
namespace curl
{
class CurlError : public error::Error
{
  static constexpr std::string_view FormatSetOptError {"Error settings value %1% to %2% parameter: %3%"};
  static constexpr std::string_view FormatError {"Curl error: %1%"};
 public:
  template <typename Value>
  CurlError(std::uint32_t opt, Value value, CURLcode code, const std::string& fileName, const std::string& funcName, std::uint32_t line)
  : Error(funcName, line, fileName, (boost::format(FormatSetOptError.data()) % value % opt % curl_easy_strerror(code)).str() )
  {
  }

  CurlError(const std::string& text, const std::string& fileName, const std::string& funcName, std::uint32_t line)
  : Error(funcName, line, fileName, (boost::format(FormatError.data()) % text ).str())
  {
  }

  CurlError(CURLcode code, const std::string& fileName, const std::string& funcName, std::uint32_t line)
  : Error(funcName, line, fileName, (boost::format(FormatError.data()) % curl_easy_strerror(code)).str())
  {
  }


};
}  // namespace curl
}  // namespace common

#define THROW_CURL_SETOPT_ERROR(opt,value,curl_code) \
  throw common::curl::CurlError(opt, value, curl_code, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define THROW_CURL_ERROR(t) \
  throw common::curl::CurlError(t, __FILE__, __PRETTY_FUNCTION__, __LINE__)

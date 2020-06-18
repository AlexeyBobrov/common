/** @file error.h
 *  @brief The define class Error
 *  @author Bobrov A.E.
 *  @date 17.10.2019
 */
#pragma once

#include <cstddef>
#include <string>
#include <stdexcept>

#include <cmntype/config.h>

namespace common
{
namespace error
{
class Error : public std::runtime_error
{
 public:
  Error(const std::string& funcName, std::uint32_t line, const std::string& fileName, const std::string& text);
  const std::string& GetFunction() const noexcept;
  uint32_t GetLine() const noexcept;
  const std::string& GetFileName() const noexcept;
  const std::string& GetText() const noexcept;
  virtual ~Error() = default;

 private:
  std::string funcName_;
  std::uint32_t line_;
  std::string fileName_;
  std::string text_;
};

template <typename ErrorType>
void ThrowIfError(const ErrorType& err, const std::string& funcName, std::uint32_t line, const std::string& fileName)
{
  if (err)
  {
    throw Error(funcName, line, fileName, err.message());
  }
}

}  // namespace error
}  // namespace common


#define THROW_COMMON_ERROR(text) \
  throw common::error::Error(FUNCNAME, __LINE__, __FILE__, text)

#define THROW_IF_ERROR(ec) common::error::ThrowIfError(ec, FUNCNAME, __LINE__, __FILE__)


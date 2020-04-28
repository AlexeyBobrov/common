/** @file utility.cpp
 *  @brief The implementation utility module
 *  @author Bobrov A.E.
 *  @date 11.10.2019
 */

// boost
#include <boost/format.hpp>

// this
#include <error/error.h>
#include <logger/utility.h>

namespace common
{
namespace logger
{
namespace utility
{
std::string_view SeverityToText(Severity severity)
{
  switch (severity)
  {
    case Severity::fatal:
      return Fatal;
    case Severity::critical:
      return Critical;
    case Severity::error:
      return Error;
    case Severity::info:
      return Info;
    case Severity::warning:
      return Warning;
    case Severity::debug:
      return Debug;
    case Severity::trace:
      return Trace;
  }

  THROW_COMMON_ERROR((boost::format("Unknown severity '%1%'") % static_cast<int>(severity)).str());
}

Severity SeverityFromText(std::string_view name)
{
  if (name == Critical)
  {
    return Severity::critical;
  }
  else if (name == Fatal)
  {
    return Severity::fatal;
  }
  else if (name == Error)
  {
    return Severity::error;
  }
  else if (name == Info)
  {
    return Severity::info;
  }
  else if (name == Warning)
  {
    return Severity::warning;
  }
  else if (name == Debug)
  {
    return Severity::debug;
  }
  else if (name == Trace)
  {
    return Severity::trace;
  }

  THROW_COMMON_ERROR((boost::format("Unknown name severity '%1%'") % name).str());
}
}  // namespace utility
}  // namespace logger
}  // namespace common

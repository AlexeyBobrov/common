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
using namespace std::literals::string_view_literals;

constexpr auto FatalText = "FATAL"sv;
constexpr auto CriticalText = "CRITICAL"sv;
constexpr auto ErrorText = "ERROR"sv;
constexpr auto InfoText = "INFO"sv;
constexpr auto WarningText = "WARNING"sv;
constexpr auto DebugText = "DEBUG"sv;
constexpr auto TraceText = "TRACE"sv;

std::string_view SeverityToText(Severity severity)
{
  switch (severity)
  {
    case Severity::fatal:
      return FatalText;
    case Severity::critical:
      return CriticalText;
    case Severity::error:
      return ErrorText;
    case Severity::info:
      return InfoText;
    case Severity::warning:
      return WarningText;
    case Severity::debug:
      return DebugText;
    case Severity::trace:
      return TraceText;
  }

  THROW_COMMON_ERROR((boost::format("Unknown severity '%1%'") % static_cast<int>(severity)).str());
}

Severity SeverityFromText(std::string_view name)
{
  if (name == CriticalText)
  {
    return Severity::critical;
  }
  else if (name == FatalText)
  {
    return Severity::fatal;
  }
  else if (name == ErrorText)
  {
    return Severity::error;
  }
  else if (name == InfoText)
  {
    return Severity::info;
  }
  else if (name == WarningText)
  {
    return Severity::warning;
  }
  else if (name == DebugText)
  {
    return Severity::debug;
  }
  else if (name == TraceText)
  {
    return Severity::trace;
  }

  THROW_COMMON_ERROR((boost::format("Unknown name severity '%1%'") % name).str());
}
}  // namespace utility
}  // namespace logger
}  // namespace common

/** @file utility.h
 *  @brief The define helpers functions of logger
 *  @author Bobrov A.E.
 *  @date 10.11.2019
 */
#pragma once

// std
#include <array>
#include <string_view>

// this
#include <logger/types.h>

namespace common
{
namespace logger
{
namespace utility
{

constexpr std::string_view Fatal = "FATAL";
constexpr std::string_view Critical = "CRITICAL";
constexpr std::string_view Error = "ERROR";
constexpr std::string_view Info = "INFO";
constexpr std::string_view Warning = "WARNING";
constexpr std::string_view Debug = "DEBUG";
constexpr std::string_view Trace = "TRACE";

/** @brief Get text name severity from enum
 *  @param severity
 *  @return text name
 */
std::string_view SeverityToText(Severity severity);

/** @brief Get severity from text
 *  @param name - text name of severity
 *  @return severity
 */
Severity SeverityFromText(std::string_view name);

}  // namespace utility
}  // namespace logger
}  // namespace common

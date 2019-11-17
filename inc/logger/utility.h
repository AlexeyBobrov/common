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

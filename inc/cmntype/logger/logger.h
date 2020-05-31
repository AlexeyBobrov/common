/** @file logger.h
 *  @brief The define logger class
 *  @author Bobrov A.E.
 *  @date 12.09.2016
 */
#ifndef COMMON_LOGGER_LOGGER_H_
#define COMMON_LOGGER_LOGGER_H_

// Boost
// logger
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

// this
#include <cmntype/config.h>
#include <cmntype/logger/types.h>

namespace common
{
namespace logger
{
class Logger final
{
 public:
  /// @brief default initialization logger
  static void Init();
  /// @brief initialization logger from filename
  /// @param filename - filename configuration
  static void InitFromFile(const filesystem::path &filename);
  /// @brief deinit logger
  static void DeInit();
};
}  // namespace logger
}  // namespace common

/// @brief boost global logger
BOOST_LOG_GLOBAL_LOGGER(Logger, common::logger::SeverityLogger)

/// @brief for debug, trace message
#define BOOST_LOG_SEV_ADD(lg, sv)                                                                             \
  BOOST_LOG_SEV(lg, sv) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) \
                        << boost::log::add_value("Function", BOOST_CURRENT_FUNCTION)

/// @brief information message
#define LOG_INFO(lg) BOOST_LOG_SEV(lg, common::logger::Severity::info)

/// @brief warning message
#define LOG_WARNING(lg) BOOST_LOG_SEV(lg, common::logger::Severity::warning)

/// @brief error message
#define LOG_ERROR(lg) BOOST_LOG_SEV(lg, common::logger::Severity::error)

/// @brief critical message
#define LOG_CRITICAL(lg) BOOST_LOG_SEV(lg, common::logger::Severity::critical)

/// @brief fatal message
#define LOG_FATAL(lg) BOOST_LOG_SEV(lg, common::logger::Severity::fatal)

/// @brief debug message
#define LOG_DEBUG(lg) BOOST_LOG_SEV_ADD(lg, common::logger::Severity::debug)

/// @brief trace message
#define LOG_TRACE(lg) BOOST_LOG_SEV_ADD(lg, common::logger::Severity::trace)

/// @brief stdout message
#define LOG_STDOUT_TRACE(text)                                                  \
    std::cout << "[" << __PRETTY_FUNCTION__ << "][" << __LINE__ << "]:  "       \
    << text << std::endl

#endif

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

// filesystem
#include <boost/filesystem.hpp>

// this
#include <logger/types.h>

namespace common
{
namespace logger
{
class Logger final
{
 public:
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  ~Logger();
  static void Init();
  static void InitFromFile(const boost::filesystem::path &filename);
};
}  // namespace logger
}  // namespace common

BOOST_LOG_GLOBAL_LOGGER(Logger, common::logger::SeverityLogger)

#define BOOST_LOG_SEV_ADD(lg, sv)                                                                             \
  BOOST_LOG_SEV(lg, sv) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) \
                        << boost::log::add_value("Function", BOOST_CURRENT_FUNCTION)

#define LOG_INFO(lg) BOOST_LOG_SEV(lg, common::logger::Severity::info)

#define LOG_WARNING(lg) BOOST_LOG_SEV(lg, common::logger::Severity::warning)

#define LOG_ERROR(lg) BOOST_LOG_SEV(lg, common::logger::Severity::error)

#define LOG_CRITICAL(lg) BOOST_LOG_SEV(lg, common::logger::Severity::critical)

#define LOG_FATAL(lg) BOOST_LOG_SEV(lg, common::logger::Severity::fatal)

#define LOG_DEBUG(lg) BOOST_LOG_SEV_ADD(lg, common::logger::Severity::debug)

#define LOG_TRACE(lg) BOOST_LOG_SEV_ADD(lg, common::logger::Severity::trace)

#endif

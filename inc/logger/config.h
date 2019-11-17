/** @file log_config.h
 * @brief the logger configuration
 * @author Bobrov A.E.
 * @date 09.07.2016
 */
#ifndef COMMON_LOGGER_CONFIG_H_
#define COMMON_LOGGER_CONFIG_H_

// std
#include <cstdint>
#include <map>
#include <string>
#include <string_view>

// boost
#include <boost/filesystem.hpp>

namespace common
{
namespace logger
{
namespace config
{
/** @struct Configuration
 * @brief The configuration logger
 */
struct Configuration
{
  using Attributes = std::map<std::string_view, bool>;

  struct Rotation
  {
    enum class Type
    {
      time,
      size
    };

    Type type{Type::time};
    std::uint32_t period{3600};
    std::uint64_t size{10};
  };

  enum class Time
  {
    utc,
    local
  };

  struct AttributesValues
  {
    static std::string_view process_id;
    static std::string_view thread_id;
    static std::string_view timestamp;
    static std::string_view filename;
    static std::string_view function;
    static std::string_view line;
  };

  bool stdoutput{true};
  boost::filesystem::path workdir = boost::filesystem::current_path();
  std::string filename = "geocoder_%Y-%m-%d_%H-%M-%S.%N.log";
  Rotation rotation;
  Time time_type{Time::utc};
  std::string level;
  Attributes attributes{{AttributesValues::process_id, true}, {AttributesValues::thread_id, true}, {AttributesValues::timestamp, true},
                        {AttributesValues::filename, true},   {AttributesValues::function, true},  {AttributesValues::line, true}};
};

/** @brief read configuration from file */
Configuration ReadFile(const boost::filesystem::path &filename);
}  // namespace config
}  // namespace logger
}  // namespace common

#endif

/** @file logger.cpp
 *  @brief The implementation logger
 *  @author Bobrov A.E.
 *  @date 12.09.2016
 */

// declare
#include <cmntype/logger/logger.h>

// std
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

// boost
//  common
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

//  logger
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

//  memory
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
//  ptree
#include <boost/property_tree/ptree.hpp>
//  date_time
#include <boost/date_time/posix_time/posix_time.hpp>
//  utils
#include <boost/core/null_deleter.hpp>

// this
#include <cmntype/config.h>
#include <cmntype/logger/config.h>
#include <cmntype/logger/utility.h>

// error
#include <cmntype/error/error.h>

#include <boost/phoenix/bind.hpp>

BOOST_LOG_GLOBAL_LOGGER_INIT(Logger, common::logger::SeverityLogger)
{
  common::logger::SeverityLogger lg;
  return lg;
}

namespace common
{
namespace logger
{
//--------------------------------------------------------------------------------------------
namespace sinks = boost::log::sinks;
namespace sources = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace log = boost::log;

//--------------------------------------------------------------------------------------------
using sev_log_t = sources::severity_logger_mt<Severity>;
using file_sink_t = sinks::synchronous_sink<sinks::text_file_backend>;
using file_sink_ptr_t = boost::shared_ptr<file_sink_t>;
using debug_sink_t = sinks::synchronous_sink<sinks::text_ostream_backend>;
using debug_sink_ptr_t = boost::shared_ptr<debug_sink_t>;
using debug_sink_ptr_t = boost::shared_ptr<debug_sink_t>;
//--------------------------------------------------------------------------------------------
template <typename Attribute>
void AddAttribute(std::string_view name)
{
  auto it = log::core::get()->add_global_attribute(name.data(), Attribute());
  if (!it.second)
  {
    // TODO: Удаление всех атрибутов при деинициализации
  }
}

template <typename Type>
void Print(std::ostringstream &ss, const Type &value)
{
  ss << "[" << value << "]";
}

template <typename AttributeType>
void FormatAttribute(const config::Configuration::Attributes &attrs, std::string_view name, const log::record_view &record, std::ostringstream &ss)
{
  const auto& attrIt = attrs.find(name);
  if (std::cend(attrs) != attrIt)
  {
    const auto&it = attrs.find(name);
    if (it->second)
    {
      const auto &value = log::extract<AttributeType>(name.data(), record);
      if (value)
      {
        Print(std::ref(ss), value.get());
      }
    }
  }
}

void FormatFileName(const config::Configuration::Attributes& attrs, const log::record_view &record, std::ostringstream& ss)
{
  const auto& attrFileName = config::Configuration::AttributesValues::filename;

  const auto& attrIt = attrs.find(attrFileName);
  if (std::cend(attrs) != attrIt)
  {
    if (attrIt != attrs.end())
    {
      const auto& value = log::extract<std::string>(attrFileName.data(), record);
      if (value)
      {
        filesystem::path fileName{value.get()};
        Print(std::ref(ss), fileName.filename().string());
      }
    }
  }
}
//--------------------------------------------------------------------------------------------
// configure attributes
void InitAttributes(const config::Configuration &conf)
{
  const auto &attrs = conf.attributes;

  for (const auto &attr : attrs)
  {
    using config::Configuration;

    if (attr.second)
    {
      if (attr.first == Configuration::AttributesValues::process_id)
      {
        AddAttribute<log::attributes::current_process_id>(Configuration::AttributesValues::process_id);
      }
      else if (attr.first == Configuration::AttributesValues::thread_id)
      {
        AddAttribute<log::attributes::current_thread_id>(Configuration::AttributesValues::thread_id);
      }
      else if (attr.first == Configuration::AttributesValues::timestamp)
      {
        if (conf.time_type == Configuration::Time::utc)
        {
          AddAttribute<log::attributes::utc_clock>(Configuration::AttributesValues::timestamp);
        }
        else if (conf.time_type == Configuration::Time::local)
        {
          AddAttribute<log::attributes::local_clock>(Configuration::AttributesValues::timestamp);
        }
        else
        {
          THROW_COMMON_ERROR("unknown time type of log record.");
        }
      }
      else if (attr.first == Configuration::AttributesValues::filename)
      {
      }
      else if (attr.first == Configuration::AttributesValues::function)
      {
      }
      else if (attr.first == Configuration::AttributesValues::line)
      {
      }
      else
      {
        THROW_COMMON_ERROR((boost::format("unknown name attribute '%1%'") % attr.first).str());
      }
    }
  }
}
//--------------------------------------------------------------------------------------------
// format message
void Format(const config::Configuration &conf, const log::record_view &record, log::formatting_ostream &os)
{
  auto sev = record.attribute_values()["Severity"].extract<Severity>().get();
  auto text = utility::SeverityToText(sev);

  std::ostringstream tmp;

  const auto &attrs = conf.attributes;

  FormatAttribute<boost::posix_time::ptime>(std::cref(attrs), config::Configuration::AttributesValues::timestamp, std::cref(record), std::ref(tmp));

  FormatAttribute<log::attributes::current_thread_id::value_type>(std::cref(attrs), config::Configuration::AttributesValues::thread_id,
                                                                  std::cref(record), std::ref(tmp));

  FormatAttribute<log::attributes::current_process_id::value_type>(std::cref(attrs), config::Configuration::AttributesValues::process_id,
                                                                   std::cref(record), std::ref(tmp));

  Print(std::ref(tmp), text);

  FormatAttribute<std::string>(std::cref(attrs), config::Configuration::AttributesValues::function, std::cref(record), std::ref(tmp));
  FormatFileName(attrs, record, tmp);
  FormatAttribute<int>(std::cref(attrs), config::Configuration::AttributesValues::line, std::cref(record), std::ref(tmp));

  const auto msg = record.attribute_values()["Message"].extract<std::string>().get();
  tmp << ": " << msg;

  os << tmp.str();
}
//--------------------------------------------------------------------------------------------
bool Filter(const log::attribute_value_set &set, Severity level)
{
  const auto sev = set["Severity"].extract<Severity>();
  if (sev)
  {
    return static_cast<int>(sev.get()) <= static_cast<int>(level);
  }
  return false;
}

//--------------------------------------------------------------------------------------------
file_sink_ptr_t CreateFileSink(const config::Configuration &conf)
{
  auto filename{conf.workdir};
  filename /= conf.filename;

  using file_text_backend_t = sinks::text_file_backend;
  using file_text_backend_ptr_t = boost::shared_ptr<file_text_backend_t>;

  file_text_backend_ptr_t backend;

  if (conf.rotation.type == config::Configuration::Rotation::Type::time)
  {
    backend = boost::make_shared<file_text_backend_t>(
        keywords::file_name = filename.string(),
        keywords::time_based_rotation = sinks::file::rotation_at_time_interval(boost::posix_time::seconds(conf.rotation.period)));
  }
  else if (conf.rotation.type == config::Configuration::Rotation::Type::size)
  {
    auto sizeMb = conf.rotation.size * 1024 * 1024;
    backend = boost::make_shared<file_text_backend_t>(keywords::file_name = filename.string(), keywords::rotation_size = sizeMb);
  }
  else
  {
    THROW_COMMON_ERROR("unknown type rotation");
  }

  backend->auto_flush(true);
  backend->enable_final_rotation(false);
  auto tmp = boost::make_shared<file_sink_t>(backend);
  const auto level = utility::SeverityFromText(conf.level.data());
  tmp->set_filter(std::bind(&Filter, std::placeholders::_1, level));
  auto fn = std::bind(Format, conf, std::placeholders::_1, std::placeholders::_2);
  tmp->set_formatter(fn);
  return tmp;
}
//--------------------------------------------------------------------------------------------
debug_sink_ptr_t CreateStdoutSink(const config::Configuration &conf)
{
  auto debug_backend = boost::make_shared<log::sinks::text_ostream_backend>();
  debug_backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
  debug_backend->auto_flush(true);
  auto sink = boost::make_shared<debug_sink_t>(debug_backend);
  const auto confLevel = utility::SeverityFromText(conf.level);
  sink->set_filter(std::bind(&Filter, std::placeholders::_1, confLevel));
  auto fn = std::bind(Format, conf, std::placeholders::_1, std::placeholders::_2);
  sink->set_formatter(fn);

  return sink;
}
//--------------------------------------------------------------------------------------------
std::atomic<bool> g_init{false};
//--------------------------------------------------------------------------------------------
// init logger
void InitLog(const config::Configuration &conf)
{
  // add attributes
  InitAttributes(conf);

  log::add_common_attributes();

  auto core = log::core::get();

  // create file sink
  auto file_sink = CreateFileSink(conf);

  if (conf.stdoutput)
  {
    // creating debug log
    auto debug_sink = CreateStdoutSink(conf);

    core->add_sink(debug_sink);
  }

  core->add_sink(file_sink);
}
//--------------------------------------------------------------------------------------------
/** @brief implementation of the class Logger */
void Logger::Init()
{
  // default settings
  InitLog(config::Configuration());
    
}
//--------------------------------------------------------------------------------------------
void Logger::InitFromFile(const filesystem::path &filename)
{
  if (!g_init)
  {
    const auto conf = config::ReadFile(filename);
    InitLog(conf);
    g_init = true;
  }
}
//--------------------------------------------------------------------------------------------
void Logger::DeInit()
{
  auto core = log::core::get();
  
  // Flush all logs that may have left buffered
  core->flush();

  // Remove all sinks
  core->remove_all_sinks();

  g_init = false;
}
//--------------------------------------------------------------------------------------------
}  // namespace logger
}  // namespace common

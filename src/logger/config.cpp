/** @file config.cpp
 * @brief the implementation config
 * @author Bobrov A.E.
 * @date 09.07.2016
 */
// boost
// property tree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/format.hpp>
// string
#include <boost/algorithm/string.hpp>

// std
#include <stdexcept>

// this
#include <logger/config.h>
#include <error/error.h>

namespace common
{
namespace logger
{
namespace config
{
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
//--------------------------------------------------------------------------------------------
std::string_view Configuration::AttributesValues::process_id = "ProcessID";
std::string_view Configuration::AttributesValues::thread_id = "ThreadID";
std::string_view Configuration::AttributesValues::timestamp = "TimeStamp";
std::string_view Configuration::AttributesValues::filename = "File";
std::string_view Configuration::AttributesValues::function = "Function";
std::string_view Configuration::AttributesValues::line = "Line";
//---------------------------------------------------------------------------------------------------------
Configuration ReadFile(const boost::filesystem::path &filename)
{
  if (!fs::exists(filename))
  {
    THROW_COMMON_ERROR( ( boost::format( "is not exists file '%1%'" ) % filename.string() ).str() );
  }

  pt::ptree document;

  pt::read_xml(filename.string(), document);
  Configuration conf;

  try
  {
    if (auto log_conf = document.get_child_optional("document.logger"))
    {
      conf.stdoutput = log_conf->get("stdout", false);
      conf.workdir = log_conf->get<fs::path>("workdir");
      conf.filename = log_conf->get<std::string>("filename");
      auto level = log_conf->get<std::string>("level");
      boost::to_upper(level);
      conf.level = level;

      const auto time_type = log_conf->get<std::string>("time");

      if (time_type == "utc")
      {
        conf.time_type = Configuration::Time::utc;
      }
      else if (time_type == "local")
      {
        conf.time_type = Configuration::Time::local;
      }
      else
      {
        THROW_COMMON_ERROR( ( boost::format( "invalid time type '%1%', configuration file '%2%'" ) % time_type % filename.string() ).str());
      }

      if (const auto rotation = log_conf->get_child_optional("rotation"))
      {
        conf.rotation.period = rotation->get<std::uint32_t>("period");
        conf.rotation.size = rotation->get<std::uint64_t>("size");
        const auto type = rotation->get<std::string>("type");

        if (type == "time")
        {
          conf.rotation.type = Configuration::Rotation::Type::time;
        }
        else if (type == "size")
        {
          conf.rotation.type = Configuration::Rotation::Type::size;
        }
        else
        {
          THROW_COMMON_ERROR( ( boost::format( "invalid rotation type '%1%', configuration file '%2%'" ) % type % filename.string() ).str() );
        }
      }

      for (auto &i : conf.attributes)
      {
        auto key(std::get<0>(i));
        std::string tmp(key);
        boost::to_lower(tmp);
        const auto value = log_conf->get_optional<bool>("attributes." + tmp);
        if (value)
        {
          i.second = *value;
        }
      }

      return conf;
    }
    else
    {
      THROW_COMMON_ERROR( ( boost::format("is not found section 'logger' into configuration file '%1%'" ) % filename.string() ).str() );
    }
  }
  catch (const std::runtime_error &)
  {
    throw;
  }
  catch (const std::exception &err)
  {
    THROW_COMMON_ERROR( ( boost::format( "Is not read file '%1%' (%2%)" ) % filename.string() % err.what() ).str() );
  }
}
//---------------------------------------------------------------------------------------------------------
}  // namespace config
}  // namespace logger
}  // namespace common

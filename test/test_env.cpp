/** @file test_env.cpp
 *  @brief The implementation test environment
 *  @author Bobrov A
 *  @date 29.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */

// std
#include <memory>
#include <functional>
#include <fstream>

// boost
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// this
#include <test_env.h>
#include <error/error.h>
#include <logger/logger.h>

namespace common
{
namespace test
{
  constexpr std::string_view LogConfig = 
    R"(<?xml version="1.0" encoding="UTF-8"?>
  <document>
    <logger>
      <stdout>false</stdout>
      <time>utc</time>
      <level>trace</level>
      <workdir>log</workdir>
      <filename>logger.%N.log</filename>
      <rotation>
        <type>size</type>
        <period>3600</period>
        <size>10000</size>
      </rotation>
      <attributes>
        <threadid>true</threadid>
        <processid>true</processid>
        <filename>true</filename>
        <function>true</function>
        <line>true</line>
      </attributes>
    </logger>
  </document>
        )";

namespace fs = boost::filesystem;

const fs::path config = fs::current_path() /  "test_logger.xml";

using FileDeleter = std::function<void(void *p)>;
FileDeleter fileDeleter = [](void*)
{
  boost::system::error_code ec;
  fs::remove_all(config, ec);
};

LogConfigFile::LogConfigFile()
: config_{fs::current_path() / "test_config.xml"}
{
  {
    std::ofstream fout{config_.c_str()};
    fout.write(LogConfig.data(), LogConfig.size());
    fout.close();
  }
  
  if (!fs::exists(config_))
  {
    THROW_COMMON_ERROR((boost::format("Config '%1%' is not found") % config_.string()).str());
  }
  
  common::logger::Logger::InitFromFile(config_);
}

LogConfigFile::~LogConfigFile()
{
  boost::system::error_code err;
  
  fs::remove_all(config_, err);
}

http::HttpServer TestEnvironment::httpServer_{ ip_, port_, threads_ };
curl::LibCurl TestEnvironment::curl_;

Logger::logger_type& TestEnvironment::GetLogger()
{
  return Logger::get();
}

void TestEnvironment::SetUp()
{
  httpServer_.Start();
}

void TestEnvironment::TearDown()
{
  httpServer_.Stop();
}

http::HttpServer& TestEnvironment::GetHttpServer()
{
  return httpServer_;
}

curl::LibCurl& TestEnvironment::GetCurl()
{
  return curl_;
}

}
}

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
#include <boost/format.hpp>

// this
#include <cmntype/config.h>
#include <test_env.h>
#include <cmntype/error/error.h>
#include <cmntype/logger/logger.h>

namespace common
{
namespace test
{
  constexpr std::string_view LogConfig = 
    R"(<?xml version="1.0" encoding="UTF-8"?>
  <document>
    <logger>
      <stdout>true</stdout>
      <time>local</time>
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
        <processid>false</processid>
        <filename>true</filename>
        <function>true</function>
        <line>true</line>
      </attributes>
    </logger>
  </document>
        )";

namespace fs = filesystem;

const fs::path config = fs::current_path() / "test_config.xml";

std::shared_ptr<http::HttpServer> TestEnvironment::httpServer_;//{ ip_, port_, threads_ };
curl::LibCurl TestEnvironment::curl_;


TestEnvironment::TestEnvironment()
{
  httpServer_ = std::make_shared<http::HttpServer>(ip_, port_, threads_);

  std::ofstream fout{config.c_str()};
  fout.write(LogConfig.data(), LogConfig.size());
  fout.close();
 
  common::logger::Logger::InitFromFile(config);
}

TestEnvironment::~TestEnvironment()
{
  std::error_code code;
  fs::remove_all(config, code);
}

void TestEnvironment::SetUp()
{
  httpServer_->Start(); 
  COMMON_LOG_INFO() << "Http server started";
}

void TestEnvironment::TearDown()
{
  COMMON_LOG_INFO() << "Http server stopping";
  httpServer_->Stop();
  COMMON_LOG_INFO() << "Http server stopped";  
}

std::shared_ptr<http::HttpServer> TestEnvironment::GetHttpServer()
{
  return httpServer_;
}

curl::LibCurl& TestEnvironment::GetCurl()
{
  return curl_;
}

}
}

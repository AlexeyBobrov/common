/// @file test_logger.cpp
/// @brief Testing logger
/// @author Bobrov A.E.
/// @date 20.10.2019

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gtest/gtest.h>

#include <logger/logger.h>

#include <thread>
#include <sstream>

namespace fs = boost::filesystem;
namespace logger = common::logger;
namespace pt = boost::property_tree;

constexpr auto LogConfigTest = 
R"(<?xml version="1.0" encoding="UTF-8"?>
  <document>
    <logger>
      <stdout>true</stdout>
      <time>utc</time>
      <level>fatal</level>
      <workdir>bin</workdir>
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

class LogTest : public ::testing::Test
{
 public:
  LogTest()
   : confPath_(fs::current_path() / "test_logger.xml")
  {
    std::istringstream in(LogConfigTest);
    pt::read_xml(in, config_);

    if (auto document = config_.get_child_optional("document"))
    {
      if (auto logger = document->get_child_optional("logger"))
      {
        logger->put(pt::ptree::path_type{"workdir"}, fs::current_path().string());
      }
    }

    pt::write_xml(confPath_.string(), config_);
  }
  
  ~LogTest()
  {

    fs::remove( confPath_ );
    
  }

  virtual void SetUp() override {}

  virtual void TearDown() override {}

 protected:
  pt::ptree& GetConfig() { return config_; }
  const fs::path& GetFileConfig() const { return confPath_; }
 private:
  pt::ptree config_;
  fs::path confPath_;
};

TEST_F(LogTest, InitLogger)
{
  const auto& pathConfig = GetFileConfig();

  ASSERT_TRUE(fs::exists(pathConfig));

  //ASSERT_NO_THROW(logger::Logger::InitFromFile(pathConfig));
  logger::Logger::InitFromFile(pathConfig);
  auto& log = Logger::get();
  LOG_INFO(log) << "Info";
  LOG_WARNING(log) << "Warning";
  LOG_ERROR(log) << "Error";
  LOG_CRITICAL(log) << "Critical";
  LOG_FATAL(log) << "Fatal";
  LOG_DEBUG(log) << "Debug";
  LOG_TRACE(log) << "Trace";
  //
  boost::log::core::get()->flush();
  boost::log::core::get()->remove_all_sinks();


}


/// @file test_logger.cpp
/// @brief Testing logger
/// @author Bobrov A.E.
/// @date 20.10.2019

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gtest/gtest.h>

#include <cmntype/config.h>
#include <cmntype/logger/logger.h>
#include <cmntype/logger/config.h>
#include <cmntype/logger/utility.h>
#include <cmntype/logger/types.h>
#include <test_env.h>

#include <thread>
#include <sstream>

namespace fs = filesystem;
namespace logger = common::logger;
namespace pt = boost::property_tree;
namespace config = logger::config;


class LogTest : public ::testing::Test
{
 public:
  LogTest()
  {
  }
  
  ~LogTest()
  {
  }

  virtual void SetUp() override 
  {
  }

  virtual void TearDown() override 
  {
    std::error_code err;
    fs::remove_all(ConfigFullFileName, err);
  }

 protected:
  void WriteXml(std::string_view data)
  {
    std::ofstream fout(ConfigFullFileName);
    
    fout.write(data.data(), data.size());

    fout.close();
  }

  static const fs::path ConfigFullFileName;
};

const fs::path LogTest::ConfigFullFileName = fs::current_path() / "config.xml.tmp";


TEST_F(LogTest, InitLogger)
{
  auto& log = Logger::get();
  LOG_INFO(log) << "Info";
  LOG_WARNING(log) << "Warning";
  LOG_ERROR(log) << "Error";
  LOG_CRITICAL(log) << "Critical";
  LOG_FATAL(log) << "Fatal";
  LOG_DEBUG(log) << "Debug";
  LOG_TRACE(log) << "Trace";
}

TEST_F(LogTest, SeverityToText)
{
  namespace utils = logger::utility;
  
  ASSERT_EQ(utils::Critical, utils::SeverityToText(logger::Severity::critical));
  ASSERT_EQ(utils::Fatal, utils::SeverityToText(logger::Severity::fatal));
  ASSERT_EQ(utils::Error, utils::SeverityToText(logger::Severity::error));
  ASSERT_EQ(utils::Info, utils::SeverityToText(logger::Severity::info));
  ASSERT_EQ(utils::Warning, utils::SeverityToText(logger::Severity::warning));
  ASSERT_EQ(utils::Debug, utils::SeverityToText(logger::Severity::debug));
  ASSERT_EQ(utils::Trace, utils::SeverityToText(logger::Severity::trace));

  ASSERT_THROW(utils::SeverityToText(static_cast<logger::Severity>(100)), std::exception);
}

TEST_F(LogTest, SeverityFromText)
{
  namespace utils = logger::utility;

  ASSERT_EQ(logger::Severity::fatal, utils::SeverityFromText(utils::Fatal));
  ASSERT_EQ(logger::Severity::critical, utils::SeverityFromText(utils::Critical));
  ASSERT_EQ(logger::Severity::error, utils::SeverityFromText(utils::Error));
  ASSERT_EQ(logger::Severity::info, utils::SeverityFromText(utils::Info));
  ASSERT_EQ(logger::Severity::warning, utils::SeverityFromText(utils::Warning));
  ASSERT_EQ(logger::Severity::debug, utils::SeverityFromText(utils::Debug));
  ASSERT_EQ(logger::Severity::trace, utils::SeverityFromText(utils::Trace));

  ASSERT_THROW(utils::SeverityFromText("unk"), std::exception);
}

TEST_F(LogTest, InvalidFile)
{
  ASSERT_ANY_THROW(common::logger::Logger::InitFromFile("C:\\test.txt"));
}

TEST_F(LogTest, InvalidConfig1)
{
  config::Configuration config;
  ASSERT_ANY_THROW(config = config::ReadFile(ConfigFullFileName));
}

TEST_F(LogTest, InvalidConfig2)
{
  constexpr std::string_view LogConfig = 
    R"(<?xml version="1.0" encoding="UTF-8"?>
  <document>
    <logger1>
      <stdout>false</stdout>
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
    </logger1>
  </document>
        )";

  
  WriteXml(LogConfig);

  config::Configuration config;
  ASSERT_ANY_THROW(config = config::ReadFile(ConfigFullFileName));
}

TEST_F(LogTest, InvalidConfig3)
{
  constexpr std::string_view LogConfig = 
    R"(<?xml version="1.0" encoding="UTF-8"?>
  <document>
    <logger>
      <stdout>false</stdout>
      <time>l</time>
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

  
  WriteXml(LogConfig);

  config::Configuration config;
  ASSERT_ANY_THROW(config = config::ReadFile(ConfigFullFileName));
}

TEST_F(LogTest, ValidConfig)
{
  constexpr std::string_view LogConfig = 
    R"(<?xml version="1.0" encoding="UTF-8"?>
  <document>
    <logger>
      <stdout>true</stdout>
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
        <processid>false</processid>
        <filename>true</filename>
        <function>true</function>
        <line>true</line>
      </attributes>
    </logger>
  </document>
        )";

  
  WriteXml(LogConfig);

  config::Configuration config;
  ASSERT_NO_THROW(config = config::ReadFile(ConfigFullFileName));

  ASSERT_EQ(config.filename, std::string("logger.%N.log"));
  ASSERT_TRUE(config.stdoutput);
  ASSERT_EQ(config.workdir, fs::path("log"));
  ASSERT_EQ(config.level, std::string("TRACE"));
  ASSERT_EQ(config.rotation.type, config::Configuration::Rotation::Type::size);
  ASSERT_EQ(config.rotation.size, 10000);

  ASSERT_FALSE(config.attributes[config::Configuration::AttributesValues::process_id]);
  ASSERT_TRUE(config.attributes[config::Configuration::AttributesValues::thread_id]);
  ASSERT_TRUE(config.attributes[config::Configuration::AttributesValues::timestamp]);
  ASSERT_TRUE(config.attributes[config::Configuration::AttributesValues::filename]);
  ASSERT_TRUE(config.attributes[config::Configuration::AttributesValues::function]);
  ASSERT_TRUE(config.attributes[config::Configuration::AttributesValues::line]);

}


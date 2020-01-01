/** @file test_env.h
 *  @brief The define TestEnvironment
 *  @author Bobrov A.E.
 *  @date 29.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */
#pragma once

// boost
#include <boost/filesystem.hpp>

// gtest
#include <gtest/gtest.h>

// this
#include <logger/logger.h>

namespace common
{
namespace test
{

class LogConfigFile final
{
public:
  LogConfigFile();
  LogConfigFile(const LogConfigFile&) = delete;
  LogConfigFile& operator=(const LogConfigFile&) = delete;
  LogConfigFile(LogConfigFile&&) = default;
  LogConfigFile& operator=(LogConfigFile&&) = default;
  ~LogConfigFile();
private:
  boost::filesystem::path config_;
};

class TestEnvironment final : public ::testing::Environment
{
public:
  static Logger::logger_type& GetLogger();
  virtual void SetUp() override;
  virtual void TearDown() override;
private:
  LogConfigFile config_;
};
}
}

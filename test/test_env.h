/** @file test_env.h
 *  @brief The define TestEnvironment
 *  @author Bobrov A.E.
 *  @date 29.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */
#pragma once

// gtest
#include <gtest/gtest.h>

// this
#include <cmntype/config.h>
#include <cmntype/logger/logger.h>
#include <cmntype/http/http_server.h>
#include <cmntype/curl/libcurl.h>

namespace common
{
namespace test
{

class TestEnvironment final : public ::testing::Environment
{
public:
  static std::shared_ptr<http::HttpServer> GetHttpServer();
  static curl::LibCurl& GetCurl();
  static constexpr std::string_view GetIp() { return ip_; }
  static constexpr std::uint16_t GetPort() { return port_; }
  TestEnvironment();
  ~TestEnvironment();
  virtual void SetUp() override;
  virtual void TearDown() override;
private:
  static curl::LibCurl curl_;
  static constexpr std::string_view ip_ { "127.0.0.1" };
  static constexpr std::uint16_t port_ { 8080 };
  static constexpr std::uint16_t threads_{ 4 };
  static std::shared_ptr<http::HttpServer> httpServer_;
};
}
}

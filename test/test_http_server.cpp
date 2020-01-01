/** @file test_http_server.cpp
 *  @brief The test http server
 *  @author Bobrov A.E.
 *  @date 18.12.2019
 */
// std 
#include <thread>
#include <chrono>

// boost
#include <boost/filesystem.hpp>

// gtest
#include <gtest/gtest.h>

// common
#include <http/http_server.h>
#include <logger/logger.h>

// test
#include <test_env.h>

namespace fs = boost::filesystem;
namespace env = common::test;

class HttpServerTest : public ::testing::Test
{
public:
  virtual void SetUp() override
  {
    server_.Start();
  }

  virtual void TearDown() override
  {
    server_.Stop();
  }
protected:
  common::http::HttpServer& GetServer()
  {
    return server_;
  }
  Logger::logger_type& GetLogger()
  {
    return logger_;
  }
private:
  std::string_view ip_ { "127.0.0.1" };
  std::uint16_t port_ { 8080 };
  std::uint16_t countThr_ { 2 };
  common::http::HttpServer server_ { ip_, port_, countThr_ };
  Logger::logger_type& logger_{env::TestEnvironment::GetLogger()};
};

TEST_F(HttpServerTest, ServerTest)
{
  LOG_TRACE(GetLogger()) << "Start test";
  auto handler = [](const common::http::HttpRequest& request)
  {
    common::http::HttpResponse response{boost::beast::http::status::ok, request.version()};
    response.set(boost::beast::http::field::content_encoding, "UTF-8");
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.body() = "request accepted";
    response.prepare_payload();
    return response;
  };

  GetServer().AddRequestHandler("/test", boost::beast::http::verb::get, handler);

  std::this_thread::sleep_for(std::chrono::seconds(60));
}

TEST_F(HttpServerTest, DISABLED_GetRequest)
{
}

TEST_F(HttpServerTest, DISABLED_PostRequest)
{
}

TEST_F(HttpServerTest, DISABLED_PutRequest)
{
}

TEST_F(HttpServerTest, DISABLED_DeleteRequest)
{
}

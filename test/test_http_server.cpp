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
#include <boost/format.hpp>

// gtest
#include <gtest/gtest.h>

// common
#include <http/http_server.h>
#include <logger/logger.h>

// test
#include <test_env.h>

namespace common
{
namespace test
{

namespace fs = boost::filesystem;
namespace env = common::test;

static constexpr std::string_view resource { "/test" };

struct TestData
{
  boost::beast::http::status code {boost::beast::http::status::ok};
  std::string message;
  http::RequestHandler handler;
  std::string url { ( boost::format("http://%1%:%2%%3%") 
      % TestEnvironment::GetIp() % TestEnvironment::GetPort() % resource.data() ).str() };
};

using Tests = std::map<boost::beast::http::verb, TestData>;

class HttpServerTest : public ::testing::Test
{
public:
  HttpServerTest()
  {
    // GET request
    {
      TestData test;
      test.code = boost::beast::http::status::ok;
      test.message = "GET request";
      test.handler = [test](const http::HttpRequest& request)
      {
        http::HttpResponse response{ test.code, request.version() };
        response.set(boost::beast::http::field::content_encoding, "UTF-8");
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = test.message;
        response.prepare_payload();
        return response;
      };

      tests_.emplace(boost::beast::http::verb::get, test);

      GetServer().AddRequestHandler(resource.data(), boost::beast::http::verb::get, test.handler);
    }

    // POST request
    {
      TestData test;
      test.code = boost::beast::http::status::ok;
      test.message = "POST request";
      test.handler = [test](const http::HttpRequest& request)
      {
        http::HttpResponse response{test.code, request.version()};
        response.set(boost::beast::http::field::content_encoding, "UTF-8");
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = test.message;
        response.prepare_payload();
        return response;
      };

      tests_.emplace(boost::beast::http::verb::post, test);

      GetServer().AddRequestHandler(resource.data(), boost::beast::http::verb::post, test.handler);
    }

    // PUT request
    {
      TestData test;
      test.code = boost::beast::http::status::ok;
      test.message = "PUT request";
      test.handler = [test](const http::HttpRequest& request)
      {
        http::HttpResponse response{test.code, request.version()};
        response.set(boost::beast::http::field::content_encoding, "UTF-8");
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = test.message;
        response.prepare_payload();
        return response;
      };

      tests_.emplace(boost::beast::http::verb::put, test);

      GetServer().AddRequestHandler(resource.data(), boost::beast::http::verb::put, test.handler);
    }

    // DELETE request
    {
      TestData test;
      test.code = boost::beast::http::status::ok;
      test.message = "DELETE request";
      test.handler = [test](const http::HttpRequest& request)
      {
        http::HttpResponse response{test.code, request.version()};
        response.set(boost::beast::http::field::content_encoding, "UTF-8");
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = test.message;
        response.prepare_payload();
        return response;
      };

      tests_.emplace(boost::beast::http::verb::delete_, test);

      GetServer().AddRequestHandler(resource.data(), boost::beast::http::verb::delete_, test.handler);
    }
    
  }
  virtual void SetUp() override
  {
  }

  virtual void TearDown() override
  {
  }
protected:
  http::HttpServer& GetServer()
  {
    return TestEnvironment::GetHttpServer();
  }
  
  Logger::logger_type& GetLogger()
  {
    return logger_;
  }

  curl::LibCurl& GetCurl()
  {
    return TestEnvironment::GetCurl();
  }

  const std::string& GetUrl() const
  {
    return url_;
  }

  const Tests& GetTests() const
  {
    return tests_;
  }
  
private:
  std::string url_;
  Tests tests_;
  Logger::logger_type& logger_{env::TestEnvironment::GetLogger()};
};

TEST_F(HttpServerTest, DISABLED_ServerTest)
{
}

TEST_F(HttpServerTest, GetRequest)
{
  const auto& test = GetTests().at(boost::beast::http::verb::get);
  auto& curl = GetCurl();
  auto response = curl.Get(test.url, ""); 
  ASSERT_EQ(std::get<1>(response), static_cast<long>(test.code));
  ASSERT_EQ(std::get<0>(response), test.message);  
}

TEST_F(HttpServerTest, PostRequest)
{
  const auto& test = GetTests().at(boost::beast::http::verb::post);
  auto& curl = GetCurl();
  auto response = curl.Post(test.url, "");
  ASSERT_EQ(std::get<1>(response), static_cast<long>(test.code));
  ASSERT_EQ(std::get<0>(response), test.message);
}

TEST_F(HttpServerTest, DISABLED_PutRequest)
{
  /*
  const auto& test = GetTests().at(boost::beast::http::verb::put);
  auto& curl = GetCurl();
  auto response = curl.Put(test.url, "");
  ASSERT_EQ(std::get<1>(response), static_cast<long>(test.code));
  ASSERT_EQ(std::get<0>(response), test.message);
  */
}

TEST_F(HttpServerTest, DeleteRequest)
{
  /*
  const auto& test = GetTests().at(boost::beast::http::verb::delete_);
  auto& curl = GetCurl();
  auto response = curl.Delete(test.url, "");
  ASSERT_EQ(std::get<1>(response), static_cast<long>(test.code));
  ASSERT_EQ(std::get<0>(response), test.message);
  */
}


}
}

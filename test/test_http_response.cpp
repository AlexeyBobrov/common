//! @file test_http_response.cpp
//! @brief Define module test for http response
//! @author Bobrov A.E.
//! @date 28.04.2020
//! @copyright (c) Bobrov A.E.

#include <gtest/gtest.h> 

#include <http/http_response.h>

#include <http/types.h>

namespace http = common::http;
namespace beast_http = boost::beast::http;

constexpr std::string_view content_type = "text/plain";
constexpr std::string_view content_encoding = "utf-8";

auto Check(const http::HttpResponse& response)
{
  ASSERT_EQ(response.at(beast_http::field::content_type), std::string{content_type});
}

TEST(HttpResponse, BadRequest)
{
  auto response = http::MakeResponseForBadRequest( http::HttpRequest(),
      content_type, content_encoding, std::string_view{});
  
  Check(response);

  ASSERT_EQ(response.result() , beast_http::status::bad_request );
}

TEST(HttpResponse, NotFoundRequest)
{
  auto response = http::MakeResponseForNotFound( http::HttpRequest(),
      "text/plain", "UTF-8", std::string_view{});

  Check(response);

  ASSERT_EQ( response.result(), boost::beast::http::status::not_found );
}

TEST(HttpResponse, NotAllowed)
{
  auto response = http::MakeResponseForNotAllowed(http::HttpRequest(),
      "text/plain", "UTF-8", std::string_view{});
  
  Check(response);

  ASSERT_EQ(response.result(), boost::beast::http::status::method_not_allowed);
}

TEST(HttpResponse, ServerError)
{
  auto response = http::MakeResponseForServerError(http::HttpRequest(),
      "text/plain", "UTF-8", std::string_view{});

  Check(response);

  ASSERT_EQ(response.result(), boost::beast::http::status::internal_server_error);
}

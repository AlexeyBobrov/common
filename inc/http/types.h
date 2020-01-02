/** @file types.h
 *  @brief the define user types
 *  @author Bobrov A.E.
 *  @date 20.12.2019
 */
#pragma once

// std
#include <functional>
#include <map>
#include <memory>

// boost
#include <boost/beast.hpp>

namespace common
{
namespace http
{
enum class Method
{
  UNKNOWN = -1,
  GET = 0,
  POST = 1,
  DELETE = 2,
  PUT = 3
};

using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;
using HttpRequest = boost::beast::http::request<boost::beast::http::string_body>;
using HttpResponsePtr = std::shared_ptr<HttpResponse>;
using HttpRequestPtr = std::shared_ptr<HttpRequest>;
using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;
struct Context
{
  boost::beast::http::verb method;
  RequestHandler handler;
};

using Handlers = std::multimap<std::string, Context>;

}
}

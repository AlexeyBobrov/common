/** @file http_server.h
 *  @brief The definition the class HttpServer
 *  @author Bobrov A.E.
 *  @date 03.12.2019
 */
#pragma once

// std
#include <memory>
#include <cstddef>
#include <string_view>
#include <functional>
#include <thread>
#include <list>
#include <shared_mutex>

// boost
#include <boost/asio.hpp>
#include <boost/beast.hpp>

// this
#include <http/types.h>

namespace common
{
namespace http
{

class HttpServer final
{
public:
  explicit HttpServer(const std::string_view address, std::uint16_t port, std::uint16_t threads);
  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  HttpServer(HttpServer&&);
  HttpServer& operator=(HttpServer&&);
  ~HttpServer();
  void Start();
  void Stop();
  void AddRequestHandler(const std::string_view uri, boost::beast::http::verb method, RequestHandler handler);
private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}
}

/** @file http_response.cpp
 *  @brief The implementation of the helpers http response
 *  @author Bobrov A.E.
 *  @date 30.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */

// std
#include <memory>

// this
#include <cmntype/http/http_response.h>

namespace common
{
namespace http
{

namespace beast_http = boost::beast::http;

HttpResponse MakeResponse(const HttpRequest& request,
    boost::beast::http::status status,
    std::string_view contentType, std::string_view encoding,
    std::string_view data)
{
  HttpResponse response{status, request.version()};

  response.set(beast_http::field::server, BOOST_BEAST_VERSION_STRING);
  response.set(beast_http::field::content_type, contentType.data());
  response.set(beast_http::field::content_encoding, encoding.data());
  response.keep_alive(request.keep_alive());
  response.body() = std::string(data);
  response.prepare_payload();

  return response;
}

HttpResponse MakeResponseForNotFound(const HttpRequest& request,
    std::string_view contentType, std::string_view encoding,
    std::string_view data)
{
  return MakeResponse(request, boost::beast::http::status::not_found,
      contentType, encoding, data);
}

HttpResponse MakeResponseForBadRequest(const HttpRequest& request,
    std::string_view contentType, std::string_view encoding,
    std::string_view data)
{
  return MakeResponse(request, boost::beast::http::status::bad_request,
      contentType, encoding, data);
}

HttpResponse MakeResponseForNotAllowed(const HttpRequest& request,
    std::string_view contentType, std::string_view encoding,
    std::string_view data)
{
  return MakeResponse(request, boost::beast::http::status::method_not_allowed,
      contentType, encoding, data);
}

HttpResponse MakeResponseForServerError(const HttpRequest& request,
    std::string_view contentType, std::string_view encoding,
    std::string_view data)
{
  return MakeResponse(request, boost::beast::http::status::internal_server_error,
      contentType, encoding, data);
}
    
}
}

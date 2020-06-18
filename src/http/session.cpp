//! @file session.cpp
//
//! @brief The implementation session class
//! @author Bobrov Alexey
//! @date 17.06.2020
//! @copyright (c) Bobrov Alexey

#include <cmntype/http/session.h>
#include <cmntype/http/http_response.h>
#include <cmntype/logger/logger.h>
#include <cmntype/common/stopwatch.h>

namespace common
{
namespace http
{

Session::Dispatcher::Dispatcher(Session& self)
: self_{self}
{
}

void Session::Dispatcher::operator()(const HttpRequest& request) const
{
  const auto& uri = request.target();
  const auto& verb = request.method();

  COMMON_LOG_TRACE() << "Search handlers for uri '" << uri << "', method '" << verb << "' (total count of handlers: " << self_.handlers_.size() << ")";

  auto handlersUri = self_.handlers_.equal_range(std::string(uri));

  RequestHandler handler;

  if (std::cend(self_.handlers_) != handlersUri.first)
  {
    for (auto it = handlersUri.first; it != handlersUri.second; ++it)
    {
      if (it->second.method == verb)
      {
        handler = it->second.handler;
      }
    }
  }

  if (handler)
  {
    Stopwatch watch;
    auto response = handler(request);
    COMMON_LOG_TRACE() << "Request processing completed "  << (watch.Get() * 1000.0) << " ms";
    self_.lambda_(std::move(response));
  }
  else
  {
    COMMON_LOG_WARNING() << "Not found handler for uri '" << uri << "', method '" << verb << "'";
    auto response = MakeResponseForNotFound(request, "application/json", "UTF-8", "not found");
    self_.lambda_(std::move(response));  
  }
  
  COMMON_LOG_TRACE() << "Complete dispatch request";
}

Session::Session(boost::asio::ip::tcp::socket&& socket, const Handlers& handlers)
  : stream_{std::move(socket)}
  , handlers_{handlers}
  , lambda_{*this}
  , dispatcher_{*this}
{

}

void Session::Run()
{
  boost::asio::dispatch(stream_.get_executor(),
      boost::beast::bind_front_handler(&Session::DoRead, shared_from_this()));
}
void Session::DoRead()
{
  req_ = {};
  stream_.expires_after(std::chrono::seconds(30));

  boost::beast::http::async_read(stream_, buffer_, req_,
      boost::beast::bind_front_handler(&Session::HandleRead, shared_from_this()));
}

void Session::HandleRead(boost::beast::error_code ec, std::size_t bytesTransferred)
{
  boost::ignore_unused(bytesTransferred);

  if (ec == boost::beast::http::error::end_of_stream)
  {
    return DoClose();
  }

  if (ec)
  {
    COMMON_LOG_ERROR() << "read: " << ec.message();
    return;
  }

  dispatcher_(req_);

}

void Session::HandleWrite(bool close, boost::beast::error_code ec, std::size_t bytesTransferred)
{
  boost::ignore_unused(bytesTransferred);

  if (ec)
  {
    COMMON_LOG_ERROR() << "write: " << ec.message();
    return;
  }

  if (close)
  {
    return DoClose();
  }

  res_ = nullptr;

  DoRead();
}

void Session::DoClose()
{
  boost::beast::error_code ec;
  stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
    
}
}

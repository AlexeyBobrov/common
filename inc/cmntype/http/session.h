//! @file session.h
//! @brief The declare http session
//! @author Bobrov Alexey
//! @date 17.06.2020
//! @copyright (c) Bobrov Alexey
#pragma once

#include <memory>

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <cmntype/http/types.h>

namespace common
{
namespace http
{
class Session : public std::enable_shared_from_this<Session>
{
  class SendLambda
  {
  public:
    explicit SendLambda(Session& self)
      : self_{self}
    {
    }

    template <bool isRequest, class Body, class Fields>
    void operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg) const
    {
      auto sp = std::make_shared<boost::beast::http::message<isRequest, Body, Fields>>(std::move(msg));

      self_.res_ = sp;

      boost::beast::http::async_write(self_.stream_, *sp, boost::beast::bind_front_handler(&Session::HandleWrite,
            self_.shared_from_this(), sp->need_eof()));

    }

  private:
    Session& self_;
  };

  class Dispatcher
  {
  public:
    explicit Dispatcher(Session& self);
    void operator()(const HttpRequest& request) const;
  private:
    Session& self_;
  };

public:
  Session(boost::asio::ip::tcp::socket&& socket, const Handlers& handlers);
  void Run();
  void DoRead();
  void DoClose();

  void HandleWrite(bool close, boost::beast::error_code ec, std::size_t bytesTransferred);
  void HandleClose();
  void HandleRead(boost::beast::error_code ec, std::size_t bytesTransferred);
private:
  boost::beast::tcp_stream stream_;
  boost::beast::flat_buffer buffer_;
  boost::beast::http::request<boost::beast::http::string_body> req_;
  std::shared_ptr<void> res_;
  Handlers handlers_;
  SendLambda lambda_;
  Dispatcher dispatcher_;
  
};
}
}

/** @file http_server.cpp
 *  @brief The implementation of the class HttpServer
 *  @author Bobrov A.E.
 *  @date 15.12.2019
 *  @copyright
 */

// std
#include <thread>
#include <shared_mutex>

// boost
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>

// this
#include <cmntype/http/http_server.h>
#include <cmntype/http/types.h>
#include <cmntype/http/http_response.h>
#include <cmntype/http/session.h>
#include <cmntype/logger/logger.h>
#include <cmntype/thread/thread_safe.h>
#include <cmntype/common/stopwatch.h>
#include <cmntype/error/error.h>


namespace common
{
namespace http
{

using Mutex = std::shared_mutex;

class Listener : public std::enable_shared_from_this<Listener>
{
public:
  explicit Listener(boost::asio::io_context& ioc,
      boost::asio::ip::tcp::endpoint endpoint, Handlers& handlers)
  : io_{ioc}
  , endpoint_{endpoint}
  , acceptor_{boost::asio::make_strand(ioc)}
  , handlers_{handlers}
  {
    
  }
  void Start()
  {
    COMMON_LOG_DEBUG() << "Starting the listener";

    boost::beast::error_code ec;
    acceptor_.open(endpoint_.protocol(), ec);
    THROW_IF_ERROR(ec);

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    THROW_IF_ERROR(ec);

    acceptor_.bind(endpoint_, ec);
    THROW_IF_ERROR(ec);

    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    THROW_IF_ERROR(ec);

    COMMON_LOG_INFO() << "Listener is started";

    stop_ = false;

    Accept();

  }

  void Stop()
  {
    COMMON_LOG_DEBUG() << "Stoping the listener";
    
    acceptor_.cancel();
    acceptor_.close();
    stop_ = true;

    COMMON_LOG_INFO() << "Listener is stopped";
  }

  void SetHandlers(const Handlers& handlers)
  {
    std::unique_lock<Mutex> lock{m_};

    handlers_ = handlers;
  }

private:
  void Accept()
  {
    acceptor_.async_accept(boost::asio::make_strand(io_), boost::beast::bind_front_handler(&Listener::OnAccept, shared_from_this()));
  }
  
  void OnAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
  {
    if (ec)
    {
      COMMON_LOG_ERROR() << "accept: " << ec.message();
      if (stop_)
      {
        return;
      }
    }
    else
    {
      std::shared_ptr<Session> session;
      {
        std::shared_lock<Mutex> lock{m_};
        session = std::make_shared<Session>(std::move(socket), handlers_);
      }
      session->Run();
    }

    Accept();
  }

private:
  boost::asio::io_context& io_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ip::tcp::acceptor acceptor_;
  Handlers handlers_;
  std::atomic<bool> stop_{false};
  Mutex m_;
  
};

class HttpServer::Impl final
{
public:
  Impl(const std::string_view address, std::uint16_t port, std::uint16_t threads)
  : io_{threads}
  , protocol_{boost::asio::ip::make_address(address.data()), port}
  , listener_{std::make_shared<Listener>(io_, protocol_, handlers_)}
  , countThr_{threads}
  , exit_{false}
  {
  }

  void AddRequestHandler(const std::string_view uri, boost::beast::http::verb method, RequestHandler handler)
  {
    COMMON_LOG_TRACE() << "Adding handler uri = '" << std::string(uri) << "', method = '" << method << "'";

    auto handlers = handlers_.equal_range(std::string(uri));

    auto contextIt = std::end(handlers_);

    for (auto it = handlers.first; it != handlers.second; ++it)
    {
      if (it->second.method == method)
      {
        contextIt = it;
      }
    }

    if (std::end(handlers_) == contextIt)
    {
      Context context{method, handler};
      handlers_.emplace(std::string(uri), context);
    }
    else
    {
      contextIt->second.handler = handler;
    }

    listener_->SetHandlers(handlers_);

    COMMON_LOG_TRACE() << "Total count handlers: " << handlers_.size();
  }

  void Start()
  {
    if (io_.stopped())
    {
      io_.restart();
    }

    listener_->Start();

    for (std::uint16_t i = 0; i < countThr_; i++)
    {
      threads_.create_thread([this]
          {
            while (!exit_)
            {
              io_.run();
              std::this_thread::sleep_for(waitMs_);
            }
          });
    }
  }

  void Stop()
  {
    listener_->Stop();
    io_.stop();
    exit_ = true;
    threads_.join_all();
  }

private:
  boost::asio::io_context io_;
  Handlers handlers_;
  boost::asio::ip::tcp::endpoint protocol_;
  std::shared_ptr<Listener> listener_;
  std::uint16_t countThr_;
  std::atomic<bool> exit_;
  boost::thread_group threads_;
  static constexpr std::chrono::milliseconds waitMs_{60};
};

HttpServer::HttpServer(const std::string_view address, std::uint16_t port, std::uint16_t threads)
: impl_(std::make_unique<Impl>(address, port, threads))
{
}

void HttpServer::Start()
{
  impl_->Start();
}

void HttpServer::Stop()
{
  impl_->Stop();
}

HttpServer::~HttpServer()
{
}

void HttpServer::AddRequestHandler(const std::string_view uri, boost::beast::http::verb method, RequestHandler handler)
{
  impl_->AddRequestHandler(uri, method, handler);
}

}
}


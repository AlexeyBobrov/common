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

// this
#include <cmntype/http/http_server.h>
#include <cmntype/http/types.h>
#include <cmntype/http/http_response.h>
#include <cmntype/logger/logger.h>
#include <cmntype/thread/thread_safe.h>
#include <cmntype/common/stopwatch.h>

// to delete
#include <iostream>

namespace common
{
namespace http
{
namespace
{
// send
template <typename Stream>
struct Send
{
  Stream& stream_;
  bool& close_;
  boost::beast::error_code& ec_;

  explicit Send( Stream& stream,
      bool& close,
      boost::beast::error_code& ec)
    : stream_(stream)
    , close_(close)
    , ec_(ec)
  {
  }
  
  template <bool isRequest, class Body, class Fields>
  void operator()(boost::beast::http::message<isRequest, Body, Fields>& msg) const
  {
    // Determine if we should close the connection after
    close_ = msg.need_eof();

    // We need the serializer here because the serializer requires
    // a non-const file_body, and the message oriented version of
    // http::write only works with const messages.
    boost::beast::http::serializer<isRequest, Body, Fields> sr{msg};
    boost::beast::http::write(stream_, sr, ec_);
  }
};


}
HttpServer::HttpServer(HttpServer&&) = default;
HttpServer& HttpServer::operator=(HttpServer&&) = default;

using Mutex = std::shared_mutex;

class HttpServer::Impl final
{
public:
  explicit Impl(const std::string_view address, std::uint16_t port, std::uint16_t threads)
  : ip_(address)
  , port_(port)
  , protocol_(boost::asio::ip::make_address(ip_.data()), port_)
  , pool_(threads)
  , io_{1}
  {
  }
  
  // Start http server
  void Start()
  {
    exit_ = false;

    std::thread t{std::bind(&Impl::Run, this)};
    worker_ = std::make_unique<thread::ThreadSafe>(std::move(t));
    
    COMMON_LOG_INFO() << "Start http server, ip = " << ip_.data() << ", port = " << port_;
    
  }

  // thread acceptor
  void Run()
  {
    COMMON_LOG_INFO() << "Start working thread";

    try
    {
      acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(io_, protocol_);
      auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_);
      
      COMMON_LOG_DEBUG() << "Waiting new connection";

      acceptor_->async_accept(*socket, boost::bind(&Impl::Accept, this, boost::asio::placeholders::error, socket));
      
      while (!exit_)
      {
        io_.run();

        std::this_thread::sleep_for(waitThr_);
      }
    }
    catch (const std::exception& err)
    {
      COMMON_LOG_ERROR() << "Error acceptor thread: '" << err.what() << "'";
    }

    COMMON_LOG_INFO() << "Complete working thread.";
  }

  void Accept(const boost::system::error_code& error,
      std::shared_ptr<boost::asio::ip::tcp::socket> socket)
  {
    if (error)
    {
      COMMON_LOG_ERROR() << "Error accepting connection: " << error.message();
    }

    COMMON_LOG_INFO() << "New connection accepted";

    boost::asio::post(pool_, std::bind(&Impl::DoSession, this, socket));
      
    auto socketNew = std::make_shared<boost::asio::ip::tcp::socket>(io_);
    acceptor_->async_accept(*socketNew, boost::bind(&Impl::Accept, this, boost::asio::placeholders::error, socket));
  }

  // The process session
  void DoSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
  {
    BOOST_ASSERT_MSG(socket, "Socket pointer cannot be null");

    COMMON_LOG_INFO() << "Start processing new session"; 
    
    bool close{false};
    boost::beast::error_code error;

    // buffer for read
    boost::beast::flat_buffer buffer;

    // functional object for response
    Send<boost::asio::ip::tcp::socket> sender{*socket, close, error};

    while (!exit_)
    {
      // Read a request
      HttpRequest request;
      boost::beast::http::read(*socket, buffer, request, error);

      if (error == boost::beast::http::error::end_of_stream)
      {
        break;
      }

      if (error)
      {
        COMMON_LOG_ERROR() << "Error on read: " << error.message();
        return;
      }

      // dispatch message
      Dispatch<Send<boost::asio::ip::tcp::socket>>(request, std::ref(sender));

      if (error)
      {
        COMMON_LOG_ERROR() << "Error sending response";
      }
    }

    boost::beast::error_code ec;
    socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    COMMON_LOG_INFO() << "Complete processing session";
  }

  template <typename Sender>
  void Dispatch(HttpRequest request, Sender& sender)
  {
    std::shared_lock<Mutex> lock(m_);

    const auto& uri = request.target();
    const auto& verb = request.method();

    COMMON_LOG_TRACE() << "Search handlers for uri '" << uri << "', method  '" << verb << "'";

    auto handlersUri = handlers_.equal_range(std::string(uri));
    
    RequestHandler handler;

    if (std::cend(handlers_) != handlersUri.first)
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
      COMMON_LOG_TRACE() << "Request processing completed " << (watch.Get() * 1000.0) << " ms" ;
      sender(response);
    }
    else
    {
      COMMON_LOG_WARNING() << "Not found handler for uri '" << uri << "', method '" << verb << "'";
      auto response = MakeResponseForNotFound(request, "application/json", "UTF-8", "not found");
      sender(response);
    }
    
    COMMON_LOG_TRACE() << "Complete dispatch request";
  }

  void Stop()
  {
    if (!stopped_)
    {
      exit_ = true;
      io_.stop();

      if (acceptor_)
      {
        acceptor_->cancel();
        acceptor_->close();
        acceptor_.reset();
      }
      if (worker_)
      {
        worker_->Join();
        worker_.reset();
      }

      pool_.stop();
      pool_.join();

      stopped_ = true;
    }
  }

 
  ~Impl()
  {
    Stop();
  }

  void AddRequestHandler(const std::string_view uri, boost::beast::http::verb method, RequestHandler handler)
  {
    std::unique_lock<Mutex> lock(m_);

    COMMON_LOG_TRACE() << "Adding handler for uri = '" << std::string(uri) << "', method = '" << method << "'";;

    auto handlers = handlers_.equal_range(std::string(uri));

    Handlers::iterator contextIt = std::end(handlers_);
    
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

    COMMON_LOG_TRACE() << "count handlers: " << handlers_.size();
  }
private:
  std::string_view ip_;
  std::uint16_t port_;
  boost::asio::ip::tcp::endpoint protocol_;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  std::atomic<bool> exit_ { false };
  std::atomic<bool> stopped_ { false };
  Handlers handlers_;
  boost::asio::thread_pool pool_;
  std::unique_ptr<thread::ThreadSafe> worker_;
  Mutex m_;
  boost::asio::io_service io_;

  static constexpr std::chrono::milliseconds waitThr_ {60};
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

void HttpServer::AddRequestHandler( const std::string_view uri, boost::beast::http::verb method, RequestHandler handler)
{
  impl_->AddRequestHandler(uri, method, handler);
}

}
}


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

// this
#include <http/http_server.h>
#include <http/types.h>
#include <http/http_response.h>
#include <logger/logger.h>
#include <thread/thread_safe.h>
#include <common/stopwatch.h>

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
  {
  }
  
  // Start http server
  void Start()
  {
    exit_ = false;

    std::thread t{std::bind(&Impl::Run, this)};
    thrAcceptor_ = std::make_unique<thread::ThreadSafe>(std::move(t));
    
    auto& logger = Logger::get();
    LOG_INFO(logger) << "Start http server, ip = " << ip_.data() << ", port = " << port_;
  }

  // thread acceptor
  void Run()
  {
    auto& logger = Logger::get();

    try
    {
      LOG_DEBUG(logger) << "Start acceptor thread.";

      boost::asio::io_context io{1};
      acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(io, protocol_);
      
      while (!exit_)
      {
        // This will receive the new connection
        boost::asio::ip::tcp::socket socket{io};
        
        // Block until we get a connection
        acceptor_->accept(socket);

        // Launch the session, transferring ownership of the socket
        boost::asio::post(pool_, std::bind(&Impl::DoSession, this, std::ref(socket)));

        // Timeout thread
        std::this_thread::sleep_for(waitThr_);
      }
    }
    catch (const std::exception& err)
    {
      LOG_ERROR(logger) << "Error acceptor thread: '" << err.what() << "'";
    }

    LOG_DEBUG(logger) << "Complete acceptor thread.";
  }

  // The process session
  void DoSession(boost::asio::ip::tcp::socket& socket)
  {
    auto& logger = Logger::get();

    LOG_DEBUG(logger) << "Start processing new session"; 
    
    bool close{false};
    boost::beast::error_code error;

    // buffer for read
    boost::beast::flat_buffer buffer;

    // functional object for response
    Send<boost::asio::ip::tcp::socket> sender{socket, close, error};

    while (!exit_)
    {
      // Read a request
      HttpRequest request;
      boost::beast::http::read(socket, buffer, request, error);

      if (error == boost::beast::http::error::end_of_stream)
      {
        break;
      }

      if (error)
      {
        LOG_ERROR(logger) << "Error on read: " << error.message();
        return;
      }

      // dispatch message
      Dispatch<Send<boost::asio::ip::tcp::socket>>(request, std::ref(sender));

      if (error)
      {
        LOG_ERROR(logger) << "Error sending response";
      }
    }

    boost::beast::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    LOG_DEBUG(logger) << "Complete processing session";
  }

  template <typename Sender>
  void Dispatch(HttpRequest request, Sender& sender)
  {
    std::shared_lock<Mutex> lock(m_);

    const auto& uri = request.target();
    const auto& verb = request.method();

    auto& logger = Logger::get();
    LOG_TRACE(logger) << "Search handlers for uri '" << uri << "', method  '" << verb << "'";
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
      LOG_TRACE(logger) << "Request processing completed " << (watch.Get() * 1000.0) << " ms" ;
      sender(response);
    }
    else
    {
      LOG_WARNING(logger) << "Not found handler for uri '" << uri << "', method '" << verb << "'";
      auto response = MakeResponseForNotFound(request, "application/json", "UTF-8", "not found");
      sender(response);
    }
    
    LOG_TRACE(logger) << "Complete dispatch request";
  }

  void Stop()
  {
    if (!stopped_)
    {
      exit_ = true;
      if (acceptor_)
      {
        acceptor_->cancel();
        acceptor_->close();
        acceptor_.reset();
      }
      if (thrAcceptor_)
      {
        thrAcceptor_->Join();
        thrAcceptor_.reset();
      }

    
      pool_.stop();
      pool_.join();

      auto& logger = Logger::get();
      LOG_INFO(logger) << "Stop http server";
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

    auto& logger = Logger::get();

    LOG_TRACE(logger) << "Adding handler for uri = '" << std::string(uri) << "', method = '" << method << "'";;

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

    LOG_INFO(logger) << "count handlers: " << handlers_.size();
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
  std::unique_ptr<thread::ThreadSafe> thrAcceptor_;
  Mutex m_;

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


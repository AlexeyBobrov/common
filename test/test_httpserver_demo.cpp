/** @file test_httpserver_demo.cpp
 *  @brief demo http server
 *  @author Bobrov A.E.
 *  @date 30.11.2019
 */

// std
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

// boost
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>

// gtest
#include <gtest/gtest.h>

#include <cmntype/config.h>
#include <cmntype/logger/logger.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;

namespace common
{
namespace test
{

beast::string_view mime_type(beast::string_view path)
{
  using beast::iequals;
  const auto ext = [&path] {
    const auto pos = path.rfind(".");
    if (pos == beast::string_view::npos)
    {
      return beast::string_view{};
    }
    return path.substr(pos);
  }();

  if (iequals(ext, ".htm")) return "text/html";
  if (iequals(ext, ".html")) return "text/html";
  if (iequals(ext, ".php")) return "text/html";
  if (iequals(ext, ".css")) return "text/css";
  if (iequals(ext, ".txt")) return "text/plain";
  if (iequals(ext, ".js")) return "application/javascript";
  if (iequals(ext, ".json")) return "application/json";
  if (iequals(ext, ".xml")) return "application/xml";
  if (iequals(ext, ".swf")) return "application/x-shockware-flash";
  if (iequals(ext, ".flv")) return "video/x-flv";
  if (iequals(ext, ".png")) return "image/png";
  if (iequals(ext, ".jpe")) return "image/jpeg";
  if (iequals(ext, ".jpeg")) return "image/jpeg";
  if (iequals(ext, ".jpg")) return "image/jpeg";
  if (iequals(ext, ".gif")) return "image/gif";
  if (iequals(ext, ".bmp")) return "image/bmp";
  if (iequals(ext, ".ico")) return "image/vnd.microsoft.icon";
  if (iequals(ext, ".tiff")) return "image/tiff";
  if (iequals(ext, ".tif")) return "image/tiff";
  if (iequals(ext, ".svg")) return "image/svg+xml";
  if (iequals(ext, ".svgz")) return "image/svg+xml";
  return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path)
{
  if (base.empty())
  {
    return std::string(path);
  }
  std::string result(base);
#ifdef BOOST_MSVC
  constexpr char path_separator = '\\';
  if (result.back() == path_separator)
  {
    result.resize(result.size() - 1);
    for (auto& c : result)
    {
      if (c == '/')
      {
        c = path_separator;
      }
    }
  }
#else
  constexpr char path_separator = '/';
  if (result.back() == path_separator)
  {
    result.resize(result.size() - 1);
  }
  result.append(path.data(), path.size());
#endif
  return result;
}
// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the, so the interface requires the 
// caller to pass a generic lambda for receving the response
template <class Body, class Allocator, class Send>
void handle_request(beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>>&& req,
    Send&& send)
{
  auto& log = Logger::get();
  LOG_TRACE(log) << "Start handle request";

  // Returns a bad request response
  const auto bad_request = [&req, &log](beast::string_view why)
  {
    
    LOG_TRACE(log) << "create 'bad_request' response";

    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

  // Returns a not found response
  const auto not_found = [&req, &log](beast::string_view target)
  {
    LOG_TRACE(log) << "Create 'not_found' response";

    http::response<http::string_body> res{http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
  };

  // Returns a server error response
  const auto server_error = [&req, &log](beast::string_view what)
  {
    LOG_TRACE(log) << "Create 'server_error' request";

    http::response<http::string_body> res{http::status::internal_server_error, req.version()} ;
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
  };

  // Make sure we can handle the method
  if ( req.method() != http::verb::get &&
      req.method() != http::verb::head)
  {
    return send(bad_request("Unknown HTTP-method"));
  }

  // Request path must be absolute and not contain "..".
  if (req.target().empty() ||
      req.target()[0] != '/' ||
      req.target().find("..") != beast::string_view::npos)
  {
    return send(bad_request("Illegal request-target"));
  }

  // Build the path to the requested file
  std::string path = path_cat(doc_root, req.target());
  if (req.target().back() == '/')
    path.append("index.html");

  // Attempt to open the file
  beast::error_code ec;
  http::file_body::value_type body;
  body.open(path.c_str(), beast::file_mode::scan, ec);

  // Handle the case where the file doesn't exist
  if (ec == beast::errc::no_such_file_or_directory)
  {
    return send(not_found(req.target()));
  }

  // Handle an unknown error
  if (ec)
  {
    return send(server_error(ec.message()));
  }

  // Cache the size since we need it after the move
  const auto size = body.size();
  // Respond to HEAD request
  if(req.method() == http::verb::head)
  {
    LOG_TRACE(log) << "Create 'Get (no parameter)' response";

    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
  }

  LOG_TRACE(log) << "Create 'GET (with parameter)' response'";
  // Respond to GET request
  http::response<http::file_body> res{
    std::piecewise_construct,
    std::make_tuple(std::move(body)),
    std::make_tuple(http::status::ok, req.version())};
  
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, mime_type(path));
  res.content_length(size);
  res.keep_alive(req.keep_alive());
  return send(std::move(res));
}

// Report a failure
void fail(beast::error_code ec, const char* what)
{
  std::cerr << what << ": " << ec.message() << "\n";
}

// This is the C++11 equivalent of a generic lambda.
// The function object is used to send an HTTP message
template <typename Stream>
struct send_lambda
{
  Stream& stream_;
  bool& close_;
  beast::error_code& ec_;

  explicit send_lambda( Stream& stream,
      bool& close,
      beast::error_code& ec)
    : stream_(stream)
    , close_(close)
    , ec_(ec)
  {
  }
  
  template <bool isRequest, class Body, class Fields>
  void operator()(http::message<isRequest, Body, Fields>&& msg) const
  {
    auto& log = Logger::get();

    LOG_TRACE(log) << "sending response";

    // Determine if we should close the connection after
    close_ = msg.need_eof();

    // We need the serializer here because the serializer requires
    // a non-const file_body, and the message oriented version of
    // http::write only works with const messages.
    http::serializer<isRequest, Body, Fields> sr{msg};
    http::write(stream_, sr, ec_);
  }
};

// Handles an HTTP server connection
void do_session(tcp::socket& socket,
    std::shared_ptr<const std::string> const& doc_root)
{
  auto &log = Logger::get();

  LOG_TRACE(log) << "Start processing session.";

  bool close = false;
  beast::error_code ec;

  // This buffer is required to persist across reads
  beast::flat_buffer buffer;

  // This lambda is used to send messages
  send_lambda<tcp::socket> lambda{socket, close, ec};

  for (;;)
  {
    // Read a request
    http::request<http::string_body> req;
    http::read(socket, buffer, req, ec);
    if (ec == http::error::end_of_stream)
    {
      break;
    }
    if (ec)
    {
      return fail(ec, "read");
    }

    // Send the response
    handle_request(*doc_root, std::move(req), lambda);
    if (ec)
    {
      return fail(ec, "write");
    }
    if (close)
    {
      // The means we should close the connection, usually because
      // the response indicated the "Connection: close" semantic.
      break;
    }
  }

  // Send a TCP shutdown
  socket.shutdown(tcp::socket::shutdown_send, ec);

  // At this point the connection is closed gracefully
}

TEST(DemoHttpServer, DISABLED_BoostHttpServer)
{
  auto& log = Logger::get();
  constexpr std::string_view ip = "127.0.0.1";
  constexpr auto port = 8080;
  const auto doc_root = std::make_shared<std::string>(".");

  LOG_INFO(log) << "Start demo http server, ip = '" << ip << "', port = '"
    << port << "', doc_root = '" << *doc_root << "'";


  //const auto address = net::ip::make_address(ip.data());

  // The io_context is required for all I/O
  net::io_context ioc{1};

  boost::asio::ip::tcp::endpoint protocol{ boost::asio::ip::make_address(ip.data()), port };
  // The acceptor receives incoming connections;
  //tcp::acceptor acceptor{ioc, {address, port}};
  auto acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(ioc, protocol); 

  for (;;)
  {
    // This will receive the new connection
    tcp::socket socket{ioc};

    // Block until we get a connection
    //acceptor.accept(socket);
    acceptor->accept(socket);

    LOG_INFO(log) << "Accept new connection";

    // Launch the session, transferring ownership of the socket
    std::thread{std::bind(&do_session, std::move(socket), doc_root)}.detach();
  }

}
}
}


/** @file libcurl.h
 * @brief the define of the class LibCurl
 * @author Bobrov A.E.
 * @date 16.07.2016
 */
#ifndef COMMON_CURL_LIBCURL_H_
#define COMMON_CURL_LIBCURL_H_

// std
#include <memory>
#include <set>
#include <string>
#include <tuple>

// boost
#include <boost/property_tree/ptree_fwd.hpp>

namespace common
{
namespace curl
{
using Headers = std::set<std::string>;

/** @class LibCurl
 * @brief The define wrapper libcurl library (curl), thread safe (internal lock)
 * @url https://curl.haxx.se//
 */
class LibCurl final
{
 public:
  using Response = std::tuple<std::string, long>;

 public:
  /** @brief default ctor */
  LibCurl();
  /** @brief disable copy semantics */
  LibCurl(const LibCurl &) = delete;
  LibCurl &operator=(const LibCurl &) = delete;
  /** @brief move semantics */
  LibCurl(LibCurl &&);
  LibCurl &operator=(LibCurl &&);
  /** @brief dtor */
  ~LibCurl();
  /** @brief http get request
   * @param url - url
   * @param content - данные
   * @return ответ
   */
  Response Get(const std::string &url, const std::string &content);
  /** @brief http post request
   * @param url - url
   * @param content - данные
   * @return ответ
   */
  Response Post(const std::string &url, const std::string &content);
  /** @brief http delete request
   *  @param url - url
   *  @param content - данные
   *  @return ответ
   */
  Response Delete(const std::string &url, const std::string &content);
  /** @brief put - запрос
   *  @param url - url
   *  @param content - данные
   */
  Response Put(const std::string &url, const std::string &content);

  /** @brief enable verbose */
  void SetVerbose(bool enable = true);
  /** @brief set timeout (seconds) */
  void SetTimeOut(std::uint32_t t);
  /** @brief set connection timeout (seconds) */
  void SetConnTimeOut(std::uint32_t t);
  /** @brief set headers
   * @param encoding - encoding (UTF, CP1251)
   * @param headers - other headers
   */
  void SetHeaders(const std::string &encoding, const Headers &h);
  /** @brief set login and password
   * @param login - login
   * @param password - password
   */
  void SetUserPassw(const std::string &login, const std::string &passw);
  /** @brief escaping url
   * @param url - url
   * @return escape result
   */
  std::string EscapeUrl(const std::string &url);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace curl
}  // namespace common

#endif

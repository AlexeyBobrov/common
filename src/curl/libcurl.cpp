/** @file libcurl.cpp
  * @brief The implementation libcurl wrapper
  * @author Bobrov A.E.
  * @date 16.07.2016
  * @copyright (c) Bobrov A.E.
  */
// this
#include <curl/libcurl.h>
#include <curl/curl_error.h>

// std
#include <memory>
#include <functional>
#include <cassert>
#include <array>
#include <sstream>
#include <mutex>
#include <cstdint>

// boost
#include <boost/assert.hpp>

// curl
#include <curl/curl.h>

namespace common
{
namespace curl
{

using CurlErrorBuffer = std::array<char, CURL_ERROR_SIZE>;

// RAII for curl
using TCurlCleaner = std::function<void(CURL *)>;
using CurlPtr = std::unique_ptr<CURL, TCurlCleaner>;
auto CurlCleaner(CURL *p)
{
  BOOST_ASSERT_MSG( p, "Is null pointer" );
  curl_easy_cleanup(p);
}
using TCurlListCleaner = std::function<void(curl_slist *)>;
using CurlListPtr = std::unique_ptr<curl_slist, TCurlListCleaner>;
auto CurlListCleaner(curl_slist *t)
{
  if (t)
  {
    curl_slist_free_all(t);
  }
}

using Locker = std::unique_lock<std::mutex>;

/** @class CurlGlobalInitializator
  * @brief the initalizator class
  */
class CurlGlobalInitializator final
{
public:
  CurlGlobalInitializator()
  {
    auto ret = curl_global_init(CURL_GLOBAL_ALL);

    if (CURLE_OK == ret)
    {
      init_ = true;
    }
  }
  ~CurlGlobalInitializator()
  {
    if (init_)
    {
      curl_global_cleanup();
    }
  }

  operator bool() const { return init_; }
private:
  bool init_{ false };
};

LibCurl::LibCurl(LibCurl &&) = default;
LibCurl &LibCurl::operator=(LibCurl &&) = default;
LibCurl::~LibCurl() = default;

class LibCurl::Impl final
{
public:
  Impl()
    : curl_{nullptr, CurlCleaner}
    , curl_list_{nullptr, CurlListCleaner}
  {
    static CurlGlobalInitializator initializator;

    if (!initializator)
    {
      THROW_CURL_ERROR("failed global initialization");
    }

    CURL *p = curl_easy_init();
    if (!p)
    {
      THROW_CURL_ERROR("failed initizliation");
    }

    curl_.reset(p);
  }
  
  void Verbose(bool enable)
  {
    Locker locker(lock_);

    CurlSetOpt(CURLOPT_VERBOSE, static_cast<long>(enable));
  }
  
  void SetTimeOut(std::uint32_t t)
  {
    Locker locker(lock_);

    CurlSetOpt(CURLOPT_TIMEOUT, static_cast<long>(t));
  }
  
  void SetConnTimeOut(std::uint32_t t)
  {
    Locker locker(lock_);

    CurlSetOpt(CURLOPT_CONNECTTIMEOUT, static_cast<long>(t));
  }
  
  void SetUserPassw(const std::string &login, const std::string &passw)
  {
    Locker locker(lock_);
  
    CurlSetOpt(CURLOPT_USERNAME, login.data());
    CurlSetOpt(CURLOPT_PASSWORD, passw.data());
  }
  
  void SetEncoding(const std::string& encoding)
  {
    Locker locker(lock_);

    CurlSetOpt(CURLOPT_ENCODING, encoding.c_str());
  }
  
  void SetHeaders(const Headers &h)
  {
    curl_slist *headers = nullptr;
    for (const auto &i : h)
    {
      headers = curl_slist_append(headers, i.data());
    }

    curl_list_.reset(headers);
    
    CurlSetOpt(CURLOPT_HTTPHEADER, curl_list_.get());
  }

  Response Request(const std::string& url, const std::string& data)
  {
    CurlSetOpt(CURLOPT_POSTFIELDSIZE, data.size());
    CurlSetOpt(CURLOPT_POSTFIELDS, data.c_str());

    CurlSetOpt(CURLOPT_URL, url.c_str());
   
    CurlErrorBuffer errorBuffer;
    CurlSetOpt(CURLOPT_ERRORBUFFER, errorBuffer.data());

    CurlSetOpt(CURLOPT_WRITEFUNCTION, writer);
    
    std::string buffer;
    CurlSetOpt(CURLOPT_WRITEDATA, &buffer );

    auto responseCode = static_cast<long>(0);
    auto result = curl_easy_perform(curl_.get());
    
    auto retGetInfo = curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &responseCode);
    if (CURLE_OK != retGetInfo)
    {
      THROW_CURL_ERROR(retGetInfo);
    }
  
    curl_easy_reset(curl_.get());
    curl_list_.reset();

    if (CURLE_OK != result)
    {
      THROW_CURL_ERROR(result);
    }

    return std::make_tuple(std::move(buffer), responseCode);
  }

  Response Get(const std::string& url, const std::string& data)
  {
    Locker locker{lock_};

    CurlSetOpt(CURLOPT_CUSTOMREQUEST, "GET");
    return Request(url, data);
  }

  Response Post(const std::string& url, const std::string& data)
  {
    Locker locker{lock_};

    CurlSetOpt(CURLOPT_CUSTOMREQUEST, "POST");

    return Request(url, data);
  }

  Response Put(const std::string& url, const std::string& data)
  {
    Locker locker{lock_};

    CurlSetOpt(CURLOPT_CUSTOMREQUEST, "PUT");

    return Request(url, data);
  }

  Response Delete(const std::string& url, const std::string& data)
  {
    Locker locker{lock_};

    CurlSetOpt(CURLOPT_CUSTOMREQUEST, "DELETE");

    return Request(url, data);
  }
  
  std::string EscapeUrl(const std::string &url)
  {
    Locker locker(lock_);

    char *ptr = curl_easy_escape(curl_.get(), url.data(), url.size());
    std::string result{ ptr };
    curl_free(ptr);
    return result;
  }
  
  /** @brief disable copy semantics */
  Impl(const Impl &) = delete;
  Impl &operator=(const Impl &) = delete;
  /** @brief move semantics */
  Impl(Impl &&) = default;
  Impl &operator=(Impl &&) = default;
  ~Impl() = default;
  
private:
  template <typename T>
  void curlSetOpt(std::uint32_t opt, T t, std::string &&source, std::string &&optname)
  {
    auto ret = curl_easy_setopt(curl_.get(), static_cast<CURLoption>(opt), t);
    if (CURLE_OK != ret)
    {
      throwCurlError(ret, source, optname);
    }
  }

  template <typename T>
  void CurlSetOpt(std::uint32_t opt, const T& t)
  {
    auto ret = curl_easy_setopt(curl_.get(), static_cast<CURLoption>(opt), t);
    if (CURLE_OK != ret)
    {
      THROW_CURL_SETOPT_ERROR(opt, t, ret);
    }
  }

  /** @brief callback writer function */
  static std::size_t writer(char *data, std::size_t size, std::size_t nmemb, void *buffer)
  {
    auto result = static_cast<std::size_t>(0);

    std::string *ptr = static_cast<std::string*>(buffer);

    if (ptr)
    {
      ptr->append(data, size * nmemb);
      result = size * nmemb;
    }

    return result;
  }
  
private:
  CurlPtr curl_;
  CurlListPtr curl_list_;
  std::mutex lock_;
};

LibCurl::LibCurl()
  : impl_(new Impl())
{

}

LibCurl::Response LibCurl::Get(const std::string &url, const std::string& data)
{
  return impl_->Get(url, data);
}

LibCurl::Response LibCurl::Post(const std::string &url, const std::string& data)
{
  return impl_->Post(url, data);
}

LibCurl::Response LibCurl::Put(const std::string& url, const std::string& data)
{
  return impl_->Put(url, data);
}

LibCurl::Response LibCurl::Delete(const std::string& url, const std::string& data)
{
  return impl_->Delete(url, data);
}

void LibCurl::SetVerbose(bool enable)
{
  impl_->Verbose(enable);
}

void LibCurl::SetTimeOut(std::uint32_t t)
{
  impl_->SetTimeOut(t);
}

void LibCurl::SetConnTimeOut(std::uint32_t t)
{
  impl_->SetConnTimeOut(t);
}

void LibCurl::SetEncoding(const std::string& encoding)
{
  impl_->SetEncoding(encoding);
}

void LibCurl::SetHeaders(const Headers &h)
{
  impl_->SetHeaders(h);
}

void LibCurl::SetUserPassw(const std::string &login, const std::string &passw)
{
  impl_->SetUserPassw(login, passw);
}

std::string LibCurl::EscapeUrl(const std::string &url)
{
  return impl_->EscapeUrl(url);
}

}
}

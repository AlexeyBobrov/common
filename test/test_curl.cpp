/// @file test_curl.cpp
/// @brief Implementation of testing the curl
/// @date 24.02.2020
/// @author Bobrov A.E.
/// @copyright Copyright (c) Bobrov A.E.

#include <gtest/gtest.h>

#include <cmntype/curl/curl_error.h>

TEST(TestCurl, CurlThrowSetOptError)
{
  constexpr uint32_t opt = 1;
  constexpr uint32_t value = 10;
  CURLcode code = CURLE_OK;
  const auto text = (boost::format(common::curl::CurlError::FormatSetOptError.data())
      % value % opt % "No error").str();
  try
  {
    THROW_CURL_SETOPT_ERROR(opt, value, code);
  }
  catch (const common::curl::CurlError& error)
  {
    ASSERT_EQ(error.GetFileName(), __FILE__);
    ASSERT_EQ(error.GetText(), text);
    ASSERT_EQ(error.GetLine(), 20);
    ASSERT_EQ(error.GetFunction(), __PRETTY_FUNCTION__); 
  }
}

TEST(TestCurl, CurlThrowCode)
{
  CURLcode code = CURLE_OK;
  const auto text = (boost::format(common::curl::CurlError::FormatError.data())
      % "No error").str();
  try
  {
    THROW_CURL_ERROR(code);
  }
  catch (const common::curl::CurlError& error)
  {
    ASSERT_EQ(error.GetFileName(), __FILE__);
    ASSERT_EQ(error.GetText(), text);
    ASSERT_EQ(error.GetLine(), 38);
    ASSERT_EQ(error.GetFunction(), __PRETTY_FUNCTION__); 
  }
}

TEST(TestCurl, CurlThrowText)
{
  const auto textError = "No error";
  const auto text = (boost::format(common::curl::CurlError::FormatError.data())
      % textError).str();
  try
  {
    THROW_CURL_ERROR(textError);
  }
  catch (const common::curl::CurlError& error)
  {
    ASSERT_EQ(error.GetFileName(), __FILE__);
    ASSERT_EQ(error.GetText(), text);
    ASSERT_EQ(error.GetLine(), 56);
    ASSERT_EQ(error.GetFunction(), __PRETTY_FUNCTION__); 
  }
}



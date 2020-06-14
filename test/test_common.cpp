/// @file test_common.cpp
/// @brief Implementation of testing common types
/// @author Bobrov A.E.
/// @date 27.02.2020
/// @copyright Copyright (c) Bobrov A.E.

#include <string_view>

#include <gtest/gtest.h>

#include <cmntype/error/error.h>
#include <cmntype/config.h>

TEST(TestCommon, TestCommonError)
{
  constexpr std::string_view textError = "test error";
  try
  {
    THROW_COMMON_ERROR(textError.data());
  }
  catch (const common::error::Error& err)
  {
    ASSERT_EQ(err.GetLine(), 19);
    ASSERT_EQ(err.GetFileName(), __FILE__ );
    ASSERT_EQ(err.GetFunction(), FUNCNAME );
    ASSERT_EQ(err.GetText(), textError);
  }
}

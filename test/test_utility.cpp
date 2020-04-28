//! @file test_utility.cpp
//! @brief utility testing
//! @author Bobrov A.E.
//! @date 26.04.2020
//! @copyright Bobrov A.E.
#include <gtest/gtest.h>

#include <common/utility.h>

TEST(TestUtility, TestFromEnum)
{
  constexpr auto E1 = 0;
  constexpr auto E2 = 1;
  constexpr auto E3 = 5;

  enum class EnumTest
  {
    e1 = E1,
    e2 = E2,
    e3 = E3
  };

  ASSERT_EQ(E1, common::FromEnum(EnumTest::e1));
  ASSERT_EQ(E2, common::FromEnum(EnumTest::e2));
  ASSERT_EQ(E3, common::FromEnum(EnumTest::e3));
}

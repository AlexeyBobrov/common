/* @file test_main.cpp
 * @brief the implementation entry point unit test
 * @author Bobrov A.E.
 * @date 20.09.2016
 */
#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

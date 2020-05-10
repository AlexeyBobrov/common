/* @file test_main.cpp
 * @brief the implementation entry point unit test
 * @author Bobrov A.E.
 * @date 20.09.2016
 */

// std
#include <sstream>
#include <fstream>
#include <string_view>
#include <atomic>

// boost
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// gtest
#include <gtest/gtest.h>

// this
#include <cmntype/logger/logger.h>
#include <test_env.h>


int main(int argc, char *argv[])
{
  ::testing::AddGlobalTestEnvironment(new common::test::TestEnvironment);
  ::testing::InitGoogleTest(&argc, argv);
  return  RUN_ALL_TESTS();
}

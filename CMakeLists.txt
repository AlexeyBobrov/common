cmake_minimum_required (VERSION 3.3)


include(cmake/env.cmake)
include(cmake/version.cmake)
include(cmake/external.cmake)

project (${ProjectName} VERSION ${PROJECT_FULL_VERSION})

# set specify the c++ standart
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# control where the static and shared libraries are build so that on windows
# we don't need to linker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# curl library
find_package(CURL CONFIG REQUIRED)

# boost configuration
find_package(Boost 1.70 COMPONENTS log_setup log system thread date_time filesystem program_options unit_test_framework REQUIRED)

add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions( -DBOOST_ALL_NO_LIB )

# configure file version
configure_file(
  "${PROJECT_SOURCE_DIR}/inc/${ProjectName}/common_version.h.in"
  "${PROJECT_BINARY_DIR}/common_version.h"
  )

set (EXECUTABLE_OUTPUT_PATH "../bin")

set (SOURCES
  src/curl/libcurl.cpp
  src/logger/config.cpp
  src/logger/logger.cpp
  src/logger/utility.cpp
  src/error/error.cpp
  src/thread/pool_thread.cpp
  src/http/session.cpp
  src/http/http_server.cpp
  src/http/http_response.cpp
  )

set (SOURCES_TEST
    ${SOURCES}
    test/test_logger.cpp
    test/test_http_server.cpp
    test/test_env.cpp
    test/test_curl.cpp
    test/test_thread.cpp
    test/test_common.cpp
    test/test_utility.cpp
    test/test_http_response.cpp
    )

set (LIBRARIES
    ${Boost_LIBRARIES}
    CURL::libcurl
    )

set (LIBRARIES_TEST
  ${LIBRARIES}
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  )

add_library(${ProjectName} ${SOURCES})

target_include_directories(${ProjectName}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/>
  $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${CURL_INCLUDE_DIRS}>
  $<INSTALL_INTERFACE:include/>
  )

set_target_properties(${ProjectName} PROPERTIES 
  POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
  )

if (MSVC)
    target_compile_options(${ProjectName} PUBLIC "/Zc:__cplusplus")
    target_compile_features(${ProjectName} PUBLIC cxx_std_17)
endif()


target_link_libraries(${ProjectName} PUBLIC ${LIBRARIES})

option(BUILD_WITHOUT_TESTS "Build without the tests" OFF)

if (NOT BUILD_WITHOUT_TESTS)
    include(cmake/test.cmake)
endif()

include(cmake/install.cmake)

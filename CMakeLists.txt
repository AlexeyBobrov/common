cmake_minimum_required (VERSION 3.1)

include(env.cmake)
include(version.cmake)

project (${ProjectName} VERSION ${MAJOR}.${MINOR}.${BUILD})


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
find_package(curl)

# adding external components
#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/external.cmake)

message(STATUS "boost include dirs: ${Boost_INCLUDE_DIRS}")
# boost configuration
find_package(Boost 1.68 COMPONENTS log_setup log system thread date_time filesystem program_options unit_test_framework REQUIRED)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME ON)
set (Boost_USE_MULTITHREAD ON)
add_definitions(-DBOOST_LOG_DYN_LINK)


# configure file version
configure_file(
  "${PROJECT_SOURCE_DIR}/inc/common_version.h.in"
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
  src/http/http_server.cpp
  src/http/http_response.cpp
  )

set (SOURCES_TEST
    ${SOURCES}
    test/test_logger.cpp
    test/test_httpserver_demo.cpp
    test/test_http_server.cpp
    test/test_env.cpp
    )

  set (LIBRARIES
      pthread
      ${CURL_LIBRARIES}
      ${Boost_LIBRARIES}
      ${GTEST_BOTH_LIBRARIES}
      ${GMOCK_BOTH_LIBRARIES}
      )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wall -v -g")
  #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wall -g")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
  message(FATAL_ERROR "Unknown compiler, compiler id = ${CMAKE_CXX_COMPILER_ID}")
endif()

add_library(${ProjectName} ${SOURCES})

target_link_options(${ProjectName} PUBLIC "-L/usr/local/gcc-9.2/libexec/")

target_include_directories(${ProjectName}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${CURL_INCLUDE_DIRS}>
  $<INSTALL_INTERFACE:inc>
  )

target_link_libraries(${ProjectName} PUBLIC  ${LIBRARIES})

add_executable(${ProjectName}_test ${INCLUDES} ${SOURCES_TEST} test/test_main.cpp)

target_include_directories(${ProjectName}_test
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
  $<BUILD_INTERFACE:${CURL_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
  )
  
target_link_libraries(${ProjectName}_test ${LIBRARIES})

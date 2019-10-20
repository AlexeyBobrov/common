cmake_minimum_required (VERSION 3.0)

include(env.cmake)
include(version.cmake)

project (${ProjectName})

find_package(Boost 1.55 COMPONENTS log_setup log system thread date_time filesystem program_options unit_test_framework REQUIRED)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME ON)
set (Boost_USE_MULTITHREAD ON)

add_definitions(-DBOOST_LOG_DYN_LINK)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(
  "${PROJECT_SOURCE_DIR}/inc/common_version.h.in"
  "${PROJECT_BINARY_DIR}/common_version.h"
  )

include_directories(${PROJECT_BINARY_DIR}
    inc
    test
    ${Boost_INCLUDE_DIRS}
)

set (EXECUTABLE_OUTPUT_PATH "../lib")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../lib")
#set (RUNTIME_OUTPUT_DIRECTORY "../lib")
#set (LIBRARY_OUTPUT_DIRECTORY "../lib")
#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib")

set (INCLUDES
  inc
  test
  ${Boost_INCLUDE_DIRS}
)
set (SOURCES
  src/curl/libcurl.cpp
  src/logger/config.cpp
  src/logger/logger.cpp
  )

set (SOURCES_TEST
    ${SOURCES}
    )

  set (LIBRARIES
      pthread
      curl
      ${Boost_LIBRARIES}
      )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++17 -Wall")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
else()
  message(FATAL_ERROR "Unknown compiler, compiler id = ${CMAKE_CXX_COMPILER_ID}")
endif()

add_library(${ProjectName} ${INCLUDES} ${SOURCES})
target_link_libraries(${ProjectName} ${LIBRARIES})

add_executable(${ProjectName}_test ${INCLUDES} ${SOURCES_TEST} test/test_main.cpp)
target_link_libraries(${ProjectName}_test ${LIBRARIES})

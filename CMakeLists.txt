cmake_minimum_required (VERSION 3.1)

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
find_package(curl)

# boost configuration
find_package(Boost 1.68 COMPONENTS log_setup log system thread date_time filesystem program_options unit_test_framework REQUIRED)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME ON)
set (Boost_USE_MULTITHREAD ON)
add_definitions(-DBOOST_LOG_DYN_LINK)

# gtest
find_package(GTest REQUIRED CONFIG)

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
    )

set (LIBRARIES_TEST
  ${LIBRARIES}
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
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

target_include_directories(${ProjectName}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${CURL_INCLUDE_DIRS}>
  $<INSTALL_INTERFACE:inc>
  )

set_target_properties(${ProjectName} PROPERTIES 
  POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
  )

target_link_libraries(${ProjectName} PUBLIC ${LIBRARIES})

set_property(TARGET ${ProjectName} PROPERTY VERSION "${PROJECT_FULL_VERSION}")
set_property(TARGET ${ProjectName} PROPERTY SOVERSION "1")

# install rules
install(TARGETS ${ProjectName} 
  DESTINATION lib
  EXPORT ${ProjectName}Targets
  )

install(DIRECTORY inc/common
  inc/curl
  inc/error
  inc/http
  inc/logger
  inc/thread
  DESTINATION include
  )

# install
install(FILES "${PROJECT_BINARY_DIR}/common_version.h"
  DESTINATION include
  )

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
include(CPack)

# install configuration targets
install(EXPORT ${ProjectName}Targets
  FILE ${ProjectName}Targets.cmake
  DESTINATION lib/cmake/${ProjectName}
  )

include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}Config.cmake"
  INSTALL_DESTINATION "lib/cmake"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
  )

# install the configuration file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}Config.cmake
  DESTINATION lib/cmake/${ProjectName}
  )

# install the version file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}ConfigVersion.cmake
  DESTINATION lib/cmake/${ProjectName}
  )

# generate the export targets for the build tree
# needs to be after the install(TARGETS) command
export(EXPORT ${ProjectName}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}Targets.cmake"
  )

add_executable(${ProjectName}_test ${INCLUDES} ${SOURCES_TEST} test/test_main.cpp)

target_include_directories(${ProjectName}_test
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
  $<BUILD_INTERFACE:${CURL_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
  )
  
target_link_libraries(${ProjectName}_test ${LIBRARIES_TEST})

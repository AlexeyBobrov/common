cmake_minimum_required (VERSION 3.3)

include(cmake/env.cmake)
include(cmake/version.cmake)
include(cmake/external.cmake)

project (${ProjectName} VERSION ${PROJECT_FULL_VERSION})

include(GoogleTest)
include(CTest)


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
find_package(Boost 1.68 COMPONENTS log_setup log system thread date_time filesystem program_options unit_test_framework REQUIRED)

add_definitions(-DBOOST_LOG_DYN_LINK)

# gtest
find_package(GTest REQUIRED CONFIG)

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
    Boost::log_setup
    Boost::log
    Boost::system
    Boost::thread
    Boost::date_time
    Boost::filesystem
    Boost::program_options
    Boost::unit_test_framework
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

target_link_libraries(${ProjectName} PUBLIC ${LIBRARIES})

set_property(TARGET ${ProjectName} PROPERTY VERSION "${PROJECT_FULL_VERSION}")
set_property(TARGET ${ProjectName} PROPERTY SOVERSION "1")

# install rules
install(TARGETS ${ProjectName} 
  DESTINATION lib
  EXPORT ${ProjectName}Targets
  )

install(DIRECTORY inc/${ProjectName}/common
  inc/${ProjectName}/curl
  inc/${ProjectName}/error
  inc/${ProjectName}/http
  inc/${ProjectName}/logger
  inc/${ProjectName}/thread
  DESTINATION include/${ProjectName}
  )

# install
install(FILES 
  "${PROJECT_BINARY_DIR}/common_version.h"
  inc/${ProjectName}/config.h
  DESTINATION include/${ProjectName}
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
  INSTALL_DESTINATION "lib/cmake/${ProjectName}"
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

enable_testing()

add_executable(${ProjectName}_test ${INCLUDES} ${SOURCES_TEST} test/test_main.cpp)
target_compile_options(${ProjectName}_test
  PUBLIC 
  $<$<CXX_COMPILER_ID:Clang>: -g -O0 -Wall -fcoverage-mapping -fprofile-instr-generate>
  $<$<CXX_COMPILER_ID:GNU>: -g -O0 -Wall -fprofile-arcs -ftest-coverage>
  )

target_link_options(${ProjectName}_test
  PUBLIC 
  $<$<CXX_COMPILER_ID:Clang>:-fprofile-instr-generate>
  $<$<CXX_COMPILER_ID:GNU>:--coverage>
  )

target_include_directories(${ProjectName}_test
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
  $<BUILD_INTERFACE:${CURL_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
  )
  
target_link_libraries(${ProjectName}_test ${LIBRARIES_TEST})
gtest_discover_tests(${ProjectName}_test)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  if (NOT LLVM_PROFDATA OR NOT LLVM_COV)
    message(WARNING "Not found llvm coverage tools")
  else()
    
    add_custom_target(gcov
      COMMAND ${LLVM_PROFDATA} merge -sparse default.profraw -o ${ProjectName}_test.profdata
      COMMAND ${LLVM_COV} show ${ProjectName}_test -instr-profile=${ProjectName}_test.profdata
      COMMAND ${LLVM_COV} report ${ProjectName}_test -instr-profile=${ProjectName}_test.profdata
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  endif()
elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  find_program(GCOVR NAME gcovr PATHS /usr/bin/ /opt/local/bin)
  if (NOT GCOVR)
    message(FATAL_ERROR "Not found gcovr")
  endif()

  message(DEBUG "Gcovr program ${GCOVR}")

  add_custom_target(gcov
    COMMAND ${GCOVR} --gcov-executable=${GCOV} -r ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

endif()

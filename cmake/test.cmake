# gtest
find_package(GTest REQUIRED CONFIG)

include(GoogleTest)
include(CTest)

enable_testing()

add_executable(${ProjectName}_test ${INCLUDES} ${SOURCES_TEST} test/test_main.cpp)
target_compile_options(${ProjectName}_test
  PUBLIC 
  $<$<CXX_COMPILER_ID:Clang>: -g -O0 -Wall -fcoverage-mapping -fprofile-instr-generate>
  $<$<CXX_COMPILER_ID:GNU>: -g -O0 -Wall -fprofile-arcs -ftest-coverage>
  )

if (MSVC)
    target_compile_options(${ProjectName}_test PUBLIC "/Zc:__cplusplus")
    target_compile_features(${ProjectName}_test PUBLIC cxx_std_17)
endif()

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

include (cmake/gcov.cmake)
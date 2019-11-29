# Download and unpack googletest at configure time
configure_file("${CMAKE_SOURCE_DIR}/cmake/gtest.cmake.in" "${CMAKE_SOURCE_DIR}/3rdparty/googletest/CMakeLists.txt")
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/googletest )
if (result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/googletest )
if (result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parrent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/googletest/googletest-src
  ${CMAKE_SOURCE_DIR}/3rdparty/googletest/googletest-build
  EXCLUDE_FROM_ALL)

message( STATUS, "${gtest_SOURCE_DIR}/include")
include_directories("${gtest_SOURCE_DIR}/include")



cmake_minimum_required(VERSION 3.10)

option(USE_VCPKG "Use vcpkg package manager" ON)

if (USE_VCPKG)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

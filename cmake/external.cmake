cmake_minimum_required(VERSION 3.10)

option(USE_VCPKG "Use vcpkg package manager" ON)

if (USE_VCPKG)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

set(GCOV "$ENV{GCOV}")
set(LLVM_PROFDATA "$ENV{LLVM_PROFDATA}")
set(LLVM_COV "$ENV{LLVM_COV}")

message(DEBUG "GCOV=${GCOV}")
message(DEBUG "LLVM_PROFDATA=${LLVM_PROFDATA}")
message(DEBUG "LLVM_COV=${LLVM_COV}")

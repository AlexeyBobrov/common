cmake_minimum_required(VERSION 3.10)

option(USE_VCPKG "Use vcpkg package manager" ON)

if (NOT USE_VCPKG)

  option(GTEST_SYSTEM OFF)
  option(BOOST_SYSTEM OFF)

  # add external components
  set(COMPONENTS_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/libs/components")
  set(COMPONENTS_ARCHIVE "${COMPONENTS_PREFIX}/libs")
  set(COMPONENTS_MODULES "${COMPONENTS_PREFIX}/modules")

  list(APPEND CMAKE_MODULE_PATH ${COMPONENTS_MODULES})

  set(Boost_NO_SYSTEM_PATHS ON)
  set(BOOST_INSTALL_DIR ${PROJECT_BINARY_DIR}/stage)
  include(${COMPONENTS_PREFIX}/components.cmake)

else()
  
  if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  endif()

endif()
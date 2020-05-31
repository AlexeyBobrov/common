//! @file config.h
//! @brief Build configuration, libraries used
//! @author Bobrov A.E.
//! @date 31.05.2020
//! @copyright (c) Bobrov A.E.
#pragma once

#if __cplusplus >= 201703L

#include <filesystem>

namespace filesystem = std::filesystem;

#else

#include <boost/filesystem.hpp>

namespace filesystem = boost::filesystem;

#endif

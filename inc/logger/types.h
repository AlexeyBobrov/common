/** @file types.h
  * @brief the define log types
  * @author Bobrov A.E.
  * @date 09.07.2016
  */
#ifndef COMMON_LOGGER_TYPES_H_
#define COMMON_LOGGER_TYPES_H_

// boost
#include <boost/property_tree/ptree_fwd.hpp>

namespace common
{
namespace logger
{
using Config = boost::property_tree::ptree;
}
}

#endif

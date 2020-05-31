#include <iostream>

#include <boost/core/ignore_unused.hpp>

#include <cmntype/config.h>

#include <cmntype/logger/logger.h>
#include <cmntype/logger/config.h>

int main( int argc, char* argv[] )
{
  boost::ignore_unused( argc );
  boost::ignore_unused( argv );
  
  const filesystem::path config{ "config/logger.xml" };
 
  namespace logger = common::logger;

  logger::Logger::InitFromFile(config);

  auto& log1 = Logger::get();

  BOOST_LOG_SEV( log1, logger::Severity::info ) << "Info";
  BOOST_LOG_SEV( log1, logger::Severity::critical ) << "Critical";

  boost::log::core::get()->flush();
  boost::log::core::get()->remove_all_sinks();
  
  return 0;
  
}

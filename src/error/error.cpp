/** @file error.cpp
 *  @brief The implementation class error
 *  @author Bobrov A.E.
 *  @date 17.10.2019
 */
// this
#include <error/error.h>

// boost
#include <boost/format.hpp>

namespace common
{
namespace error
{
// Error division by zero in line 30, function main(), file main.cpp
static constexpr auto FormatCommonError = "%1% in line %2%, function %3%, file %4%\n";

Error::Error(const std::string& funcName, std::uint32_t line, const std::string& fileName, const std::string& text)
 : std::runtime_error( ( boost::format( FormatCommonError ) % text % line % funcName % fileName ).str() )
 , funcName_(funcName)
 , line_(line)
 , fileName_(fileName)
 , text_(text)
{
}
const std::string& Error::GetFunction() const noexcept { return funcName_; }

std::uint32_t Error::GetLine() const noexcept { return line_; }

const std::string& Error::GetFileName() const noexcept { return fileName_; }

const std::string& Error::GetText() const noexcept { return text_; }
}
}  // namespace common

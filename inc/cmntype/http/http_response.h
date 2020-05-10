/** @file http_response.h
 *  @brief The define response types and functions
 *  @author Bobrov A.E.
 *  @date 30.12.2019
 *  @copyright Copyright (c) Bobrov A.E.
 */
#pragma once

// std
#include <string_view>

// this
#include <cmntype/http/types.h>

namespace common
{
namespace http
{

/// @brief Make response from
/// @param request - request
/// @param status - status of response
/// @param contentType - type content, 'application/json' etc
/// @param encoding - encofing data
/// @param data - content response
/// @return response
HttpResponse MakeResponse(const HttpRequest& request, 
    boost::beast::http::status status,
    std::string_view contentType, std::string_view encoding,
    std::string_view data);

/// @breif Make response for bad request
HttpResponse MakeResponseForBadRequest(const HttpRequest& request,
    std::string_view contentType, std::string_view encoding,
    std::string_view data);

/// @brief Make response for not found
HttpResponse MakeResponseForNotFound(const HttpRequest& request,
    std::string_view contentType, std::string_view encoding,
    std::string_view data);

/// `@brief Make response for server error
HttpResponse MakeResponseForServerError(const HttpRequest& request, 
    std::string_view contentType, std::string_view encoding,
    std::string_view data);

/// @brief Make response for not allowed
HttpResponse MakeResponseForNotAllowed(const HttpRequest& request, 
    std::string_view contentType, std::string_view encoding,
    std::string_view data);


}
}

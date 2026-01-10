/**
 *
 *  @file ViewHelpers.h
 *  @author Jules
 *
 *  Copyright 2024, Jules.  All rights reserved.
 *  https://github.com/an-tao/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#pragma once

#include <drogon/exports.h>
#include <trantor/utils/Date.h>
#include <string>

namespace drogon
{
namespace utils
{

class DROGON_EXPORT ViewHelpers
{
  public:
    /**
     * @brief Format a date object to a string using a format string.
     *
     * @param date The date object to format.
     * @param fmt The format string (strftime style).
     * @return std::string The formatted date string.
     */
    static std::string formatDate(const trantor::Date &date, const std::string &fmt);

    /**
     * @brief Generate a CSRF token.
     *
     * @return std::string The generated CSRF token.
     */
    static std::string generateCsrfToken();

    /**
     * @brief Escape HTML special characters to prevent XSS.
     *
     * @param str The string to escape.
     * @return std::string The escaped string.
     */
    static std::string escapeHtml(const std::string &str);
};

}  // namespace utils
}  // namespace drogon

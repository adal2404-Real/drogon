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
     * @brief Formats a date object into a string based on the format provided.
     *
     * @param date The date object to format.
     * @param fmt The format string (e.g. "%Y-%m-%d %H:%M:%S").
     * @return std::string The formatted date string.
     */
    static std::string formatDate(const trantor::Date &date, const std::string &fmt);

    /**
     * @brief Generates a cryptographically secure CSRF token.
     *
     * @return std::string The CSRF token.
     */
    static std::string generateCsrfToken();

    /**
     * @brief Escapes HTML special characters to prevent XSS.
     *
     * @param str The input string.
     * @return std::string The escaped string.
     */
    static std::string escapeHtml(const std::string &str);
};

}  // namespace utils
}  // namespace drogon

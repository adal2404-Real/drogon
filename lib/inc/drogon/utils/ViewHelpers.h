/**
 *
 *  @file ViewHelpers.h
 *
 *  Drogon
 *
 */

#pragma once

#include <drogon/exports.h>
#include <drogon/HttpViewData.h>
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
     * @brief Format a date object to a string.
     *
     * @param date The date object.
     * @param fmt The format string (strftime style).
     * @return std::string The formatted date string.
     */
    static std::string formatDate(const trantor::Date &date, const std::string &fmt);

    /**
     * @brief Escape a string for HTML.
     *
     * @param str The string to escape.
     * @return std::string The escaped string.
     */
    static std::string escapeHtml(const std::string &str);

    /**
     * @brief Generate a CSRF token (random string).
     *
     * @return std::string The generated token.
     */
    static std::string generateCsrfToken();
};

}  // namespace utils
}  // namespace drogon

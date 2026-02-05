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
    static std::string formatDate(const trantor::Date &date, const std::string &fmt);
    static std::string generateCsrfToken();
    static std::string escapeHtml(const std::string &str);
};
} // namespace utils
} // namespace drogon

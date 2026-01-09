/**
 *
 *  @file ViewHelpers.cc
 *
 *  Drogon
 *
 */

#include <drogon/utils/ViewHelpers.h>
#include <drogon/utils/Utilities.h>
#include <iomanip>
#include <sstream>

namespace drogon
{
namespace utils
{

std::string ViewHelpers::formatDate(const trantor::Date &date, const std::string &fmt)
{
    std::string ret;
    dateToCustomFormattedString(fmt, ret, date);
    return ret;
}

std::string ViewHelpers::escapeHtml(const std::string &str)
{
    return HttpViewData::htmlTranslate(str);
}

std::string ViewHelpers::generateCsrfToken()
{
    return getUuid();
}

}  // namespace utils
}  // namespace drogon

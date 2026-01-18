#include <drogon/utils/ViewHelpers.h>
#include <drogon/utils/Utilities.h>
#include <sstream>

namespace drogon
{
namespace utils
{

std::string ViewHelpers::formatDate(const trantor::Date &date, const std::string &fmt)
{
    std::string str;
    drogon::utils::dateToCustomFormattedString(fmt, str, date);
    return str;
}

std::string ViewHelpers::generateCsrfToken()
{
    return drogon::utils::getUuid();
}

std::string ViewHelpers::escapeHtml(const std::string &str)
{
    std::string buffer;
    buffer.reserve(str.size());
    for (const char c : str)
    {
        switch (c)
        {
        case '&':
            buffer.append("&amp;");
            break;
        case '\"':
            buffer.append("&quot;");
            break;
        case '\'':
            buffer.append("&apos;");
            break;
        case '<':
            buffer.append("&lt;");
            break;
        case '>':
            buffer.append("&gt;");
            break;
        default:
            buffer.push_back(c);
            break;
        }
    }
    return buffer;
}

} // namespace utils
} // namespace drogon

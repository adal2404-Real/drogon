#include <drogon/utils/ViewHelpers.h>
#include <drogon/utils/Utilities.h>

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
    // Generating a random string of 32 characters
    return drogon::utils::getUuid();
}

std::string ViewHelpers::escapeHtml(const std::string &str)
{
    std::string buffer;
    buffer.reserve(str.size() * 1.1);
    for (const char ch : str)
    {
        switch (ch)
        {
            case '&':
                buffer.append("&amp;");
                break;
            case '\"':
                buffer.append("&quot;");
                break;
            case '\'':
                buffer.append("&#39;");
                break;
            case '<':
                buffer.append("&lt;");
                break;
            case '>':
                buffer.append("&gt;");
                break;
            default:
                buffer.push_back(ch);
                break;
        }
    }
    return buffer;
}

}
}

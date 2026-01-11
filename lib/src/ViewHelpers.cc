#include <drogon/utils/ViewHelpers.h>
#include <drogon/utils/Utilities.h>
#include <vector>

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

std::string ViewHelpers::generateCsrfToken()
{
    // 32 bytes of entropy encoded as hex or base64
    return secureRandomString(32);
}

std::string ViewHelpers::escapeHtml(const std::string &str)
{
    std::string buffer;
    buffer.reserve(str.size() * 1.1);
    for (char c : str)
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

}  // namespace utils
}  // namespace drogon

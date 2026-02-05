#include <drogon/utils/ViewHelpers.h>
#include <drogon/utils/Utilities.h>

namespace drogon
{
namespace utils
{

std::string ViewHelpers::formatDate(const trantor::Date &date, const std::string &fmt)
{
    return date.toCustomFormattedStringLocal(fmt);
}

std::string ViewHelpers::generateCsrfToken()
{
    return getUuid();
}

std::string ViewHelpers::escapeHtml(const std::string &str)
{
    std::string buffer;
    buffer.reserve(str.size());
    for(size_t pos = 0; pos != str.size(); ++pos) {
        switch(str[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.push_back(str[pos]);   break;
        }
    }
    return buffer;
}

} // namespace utils
} // namespace drogon

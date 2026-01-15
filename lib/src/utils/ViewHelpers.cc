/**
 *
 *  @file ViewHelpers.cc
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

#include <drogon/utils/ViewHelpers.h>
#include <drogon/utils/Utilities.h>

namespace drogon
{
namespace utils
{

std::string ViewHelpers::formatDate(const trantor::Date &date, const std::string &fmt)
{
    return date.toCustomFormattedStringLocal(fmt, false);
}

std::string ViewHelpers::generateCsrfToken()
{
    return getUuid();
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

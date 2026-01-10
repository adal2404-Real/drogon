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
#include <iomanip>
#include <sstream>

namespace drogon
{
namespace utils
{

std::string ViewHelpers::formatDate(const trantor::Date &date, const std::string &fmt)
{
    return date.toCustomFormattedString(fmt);
}

std::string ViewHelpers::generateCsrfToken()
{
    // Use secure random string for CSRF token
    return utils::secureRandomString(32);
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
            default:   buffer.append(&str[pos], 1); break;
        }
    }
    return buffer;
}

}  // namespace utils
}  // namespace drogon

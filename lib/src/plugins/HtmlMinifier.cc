/**
 *
 *  @file HtmlMinifier.cc
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

#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/HttpAppFramework.h>
#include <string_view>
#include <algorithm>
#include <cctype>

namespace drogon
{
namespace plugin
{

// No longer using regex for main minification logic
std::regex HtmlMinifier::whitespaceRegex_;
std::regex HtmlMinifier::commentRegex_;

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    drogon::app().registerPreSendingAdvice(
        [](const drogon::HttpRequestPtr &req,
           const drogon::HttpResponsePtr &resp) {
            if (resp->getContentType() == CT_TEXT_HTML)
            {
                std::string body(resp->getBody());
                std::string minifiedBody = HtmlMinifier::minify(body);
                resp->setBody(minifiedBody);
            }
        });
}

void HtmlMinifier::shutdown()
{
}

// Helper to check if a tag name matches case-insensitively
static bool isTag(std::string_view content, size_t pos, std::string_view tagName)
{
    if (pos + tagName.length() > content.length())
        return false;

    for (size_t i = 0; i < tagName.length(); ++i)
    {
        if (std::tolower(content[pos + i]) != std::tolower(tagName[i]))
            return false;
    }
    // Check if followed by space, >, or /
    if (pos + tagName.length() < content.length())
    {
        char next = content[pos + tagName.length()];
        if (next != ' ' && next != '>' && next != '/' && next != '\n' && next != '\t' && next != '\r')
            return false;
    }
    return true;
}

std::string HtmlMinifier::minify(const std::string &content)
{
    std::string result;
    result.reserve(content.size());

    // States
    bool insideTag = false;
    bool insidePre = false;
    bool insideScript = false;
    bool insideStyle = false;
    bool insideTextarea = false;

    size_t i = 0;
    while (i < content.size())
    {
        // Check for sensitive tags start
        if (!insidePre && !insideScript && !insideStyle && !insideTextarea && content[i] == '<')
        {
            if (isTag(content, i + 1, "pre")) insidePre = true;
            else if (isTag(content, i + 1, "script")) insideScript = true;
            else if (isTag(content, i + 1, "style")) insideStyle = true;
            else if (isTag(content, i + 1, "textarea")) insideTextarea = true;

            // Handle Comments <!-- ... -->
            if (content.compare(i, 4, "<!--") == 0)
            {
                // Find end of comment
                size_t endComment = content.find("-->", i + 4);
                if (endComment != std::string::npos)
                {
                    // Check if it's an IE conditional comment, usually <!--[if ...]>
                    // If so, we might want to keep it. For now, we strip all comments.
                    // But if strict compatibility is needed, we should check.
                    // Assuming basic minification: remove comments.
                    i = endComment + 3;
                    continue;
                }
            }
        }

        // Check for sensitive tags end
        if (content[i] == '<' && content[i+1] == '/')
        {
             if (insidePre && isTag(content, i + 2, "pre")) insidePre = false;
             else if (insideScript && isTag(content, i + 2, "script")) insideScript = false;
             else if (insideStyle && isTag(content, i + 2, "style")) insideStyle = false;
             else if (insideTextarea && isTag(content, i + 2, "textarea")) insideTextarea = false;
        }

        // Logic for minification
        if (insidePre || insideScript || insideStyle || insideTextarea)
        {
            // Preserve everything inside these tags
            result += content[i];
            i++;
        }
        else
        {
            // We are in normal HTML
            if (content[i] == '<')
            {
                insideTag = true;
                result += content[i];
                i++;
            }
            else if (content[i] == '>')
            {
                insideTag = false;
                result += content[i];
                i++;

                // Peek ahead to see if we can collapse whitespace
                size_t j = i;
                while (j < content.size() && std::isspace(static_cast<unsigned char>(content[j])))
                {
                    j++;
                }

                if (j < content.size() && content[j] == '<')
                {
                    // Found > \s* < pattern.
                    // Skip the whitespace.
                    i = j;
                }
                else
                {
                    // Found > \s* text.
                    // Do not skip whitespace here.
                    // Normal loop will pick up whitespace in the next iteration.
                }
            }
            else
            {
                // Normal character
                result += content[i];
                i++;
            }
        }
    }

    return result;
}

}  // namespace plugin
}  // namespace drogon

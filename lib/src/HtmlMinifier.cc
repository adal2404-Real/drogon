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
#include <drogon/HttpTypes.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace drogon
{
namespace plugin
{

namespace
{
    // Helper to compare string case-insensitively
    bool iequals(const std::string& input, size_t pos, const std::string& target) {
        if (pos + target.size() > input.size()) return false;
        for (size_t i = 0; i < target.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(input[pos + i])) != std::tolower(static_cast<unsigned char>(target[i]))) {
                return false;
            }
        }
        return true;
    }

    bool isTagDelimiter(char c) {
        return isspace(static_cast<unsigned char>(c)) || c == '>' || c == '/';
    }
}

// A simple state machine to minify HTML while preserving content in specific tags
std::string minifyHtml(const std::string& input)
{
    std::string output;
    output.reserve(input.size());

    enum State {
        Normal,
        InTag,
        InTagQuoteDouble,
        InTagQuoteSingle,
        InScript,
        InStyle,
        InPre,
        InTextarea,
        InComment
    };

    State state = Normal;
    size_t i = 0;
    size_t n = input.size();

    while (i < n) {
        if (state == Normal) {
            // Check for start of tag
            if (input[i] == '<') {
                // Check for comment <!--
                if (i + 3 < n && input[i+1] == '!' && input[i+2] == '-' && input[i+3] == '-') {
                    state = InComment;
                    i += 4;
                    continue;
                }

                // Check for special tags start (case-insensitive) and ensure delimiter follows
                if (iequals(input, i, "<script") && (i + 7 >= n || isTagDelimiter(input[i+7]))) {
                    state = InScript;
                    output += input.substr(i, 7);
                    i += 7;
                    continue;
                }
                if (iequals(input, i, "<style") && (i + 6 >= n || isTagDelimiter(input[i+6]))) {
                    state = InStyle;
                    output += input.substr(i, 6);
                    i += 6;
                    continue;
                }
                if (iequals(input, i, "<pre") && (i + 4 >= n || isTagDelimiter(input[i+4]))) {
                    state = InPre;
                    output += input.substr(i, 4);
                    i += 4;
                    continue;
                }
                if (iequals(input, i, "<textarea") && (i + 9 >= n || isTagDelimiter(input[i+9]))) {
                    state = InTextarea;
                    output += input.substr(i, 9);
                    i += 9;
                    continue;
                }

                state = InTag;
                output += '<';
                i++;
            } else {
                // Handle whitespace in Normal state (between tags or text content)
                if (isspace(static_cast<unsigned char>(input[i]))) {
                   if (output.empty() || output.back() != ' ') {
                       output += ' ';
                   }
                   // Skip consecutive whitespace
                   while (i + 1 < n && isspace(static_cast<unsigned char>(input[i+1]))) {
                       i++;
                   }
                } else {
                    output += input[i];
                }
                i++;
            }
        } else if (state == InTag) {
            output += input[i];
            if (input[i] == '>') {
                state = Normal;
            } else if (input[i] == '"') {
                state = InTagQuoteDouble;
            } else if (input[i] == '\'') {
                state = InTagQuoteSingle;
            }
            i++;
        } else if (state == InTagQuoteDouble) {
            output += input[i];
            if (input[i] == '"') {
                state = InTag;
            }
            i++;
        } else if (state == InTagQuoteSingle) {
            output += input[i];
            if (input[i] == '\'') {
                state = InTag;
            }
            i++;
        } else if (state == InComment) {
             // Check for end of comment -->
             if (i + 2 < n && input[i] == '-' && input[i+1] == '-' && input[i+2] == '>') {
                 state = Normal;
                 i += 3; // Remove comment
             } else {
                 i++; // Skip comment content
             }
        } else {
            // Inside special tags (Script, Style, Pre, Textarea) - preserve everything
            bool matchedEnd = false;

            if (state == InScript && iequals(input, i, "</script>")) {
                state = Normal;
                output += "</script>";
                i += 9;
                matchedEnd = true;
            } else if (state == InStyle && iequals(input, i, "</style>")) {
                state = Normal;
                output += "</style>";
                i += 8;
                matchedEnd = true;
            } else if (state == InPre && iequals(input, i, "</pre>")) {
                state = Normal;
                output += "</pre>";
                i += 6;
                matchedEnd = true;
            } else if (state == InTextarea && iequals(input, i, "</textarea>")) {
                state = Normal;
                output += "</textarea>";
                i += 11;
                matchedEnd = true;
            }

            if (!matchedEnd) {
                output += input[i];
                i++;
            }
        }
    }
    return output;
}

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    // Register a pre-sending advice to minify HTML
    drogon::app().registerPreSendingAdvice([](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
        if (resp->contentType() == CT_TEXT_HTML)
        {
            auto body = resp->getBody();
            std::string bodyStr(body.data(), body.length());

            // Use the safe minifier
            bodyStr = minifyHtml(bodyStr);

            // Update the response body
            resp->setBody(bodyStr);
        }
    });
}

void HtmlMinifier::shutdown()
{
    // Nothing to do
}

}  // namespace plugin
}  // namespace drogon

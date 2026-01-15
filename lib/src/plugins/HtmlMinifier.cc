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
#include <iostream>
#include <vector>
#include <algorithm>

namespace drogon
{
namespace plugin
{

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    drogon::app().registerPreSendingAdvice(
        [](const drogon::HttpRequestPtr &req,
           const drogon::HttpResponsePtr &resp) {
            if (resp->contentType() == CT_TEXT_HTML)
            {
                auto body = resp->getBody();
                std::string minified = HtmlMinifier::minify(std::string(body));
                resp->setBody(std::move(minified));
            }
        });
}

void HtmlMinifier::shutdown()
{
}

std::string HtmlMinifier::minify(const std::string &html)
{
    std::string output;
    output.reserve(html.size());

    enum State {
        CONTENT,
        TAG,
        SENSITIVE
    };

    State state = CONTENT;
    std::string sensitiveTagName;
    size_t i = 0;
    size_t len = html.length();

    auto isWhitespace = [](char c) {
        return c == ' ' || c == '\n' || c == '\t' || c == '\r';
    };

    while (i < len)
    {
        if (state == CONTENT)
        {
            if (html[i] == '<')
            {
                // Check if it is a start of a sensitive tag
                bool isSensitive = false;
                static const std::vector<std::string> sensitiveTags = {"pre", "script", "style", "textarea"};

                for (const auto& tag : sensitiveTags) {
                    if (i + 1 + tag.length() <= len) {
                        bool match = true;
                        for(size_t j=0; j<tag.length(); ++j) {
                            if (tolower(html[i + 1 + j]) != tag[j]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) {
                             char next = (i + 1 + tag.length() < len) ? html[i + 1 + tag.length()] : 0;
                             if (next == '>' || isWhitespace(next) || next == '/') { // <script> or <script ... or <script/>
                                 isSensitive = true;
                                 sensitiveTagName = tag;
                                 break;
                             }
                        }
                    }
                }

                if (isSensitive)
                {
                    state = SENSITIVE;
                    // Check if it is a self closing tag like <script ... /> (rare for these tags but possible in XHTML)
                    // If we just switch to SENSITIVE, we assume we need to wait for </tag>.
                    // But first we must consume the opening tag in TAG state to handle attributes.
                    // Actually, let's treat the opening tag as a normal TAG first,
                    // and switch to SENSITIVE only after we exit that TAG.

                    // Complication: The "TAG" state needs to know if it should transition to SENSITIVE or CONTENT upon '>'.
                    state = TAG;
                    sensitiveTagName = sensitiveTagName; // Keep it
                    // We need to know we are entering sensitive mode after this tag.
                    // Let's use a flag.
                }
                else
                {
                    state = TAG;
                    sensitiveTagName = ""; // Clear
                }

                // Correction: My state machine logic was slightly flawed.
                // Let's restart the loop with a cleaner approach.
                // We will handle the opening tag logic inside TAG state or peeking.
                // But to save time and complexity, let's use the previous logic where we enter SENSITIVE immediately
                // BUT we still need to parse through the opening tag.

                // Let's do this:
                // If we detect a start of tag, we go to TAG state.
                // Upon exiting TAG (seeing '>'), we check if the tag name we just parsed was sensitive.
                // If so, we go to SENSITIVE state.
                // If not, we go to CONTENT state.

                state = TAG;
                output.push_back(html[i]);
                i++;

                // We need to capture the tag name being parsed in TAG state.
                // But we can just rely on the previous logic:
                // check if it IS sensitive start right now.
                if (isSensitive) {
                    // We found <script...
                    // We are in TAG state now.
                    // But we want to ensure we track that we ARE entering a sensitive block.
                    // Let's mark it.
                } else {
                    sensitiveTagName = "";
                }
            }
            else if (isWhitespace(html[i]))
            {
                // Collapse whitespace
                while (i < len && isWhitespace(html[i])) {
                    i++;
                }

                bool prevIsTagEnd = (!output.empty() && output.back() == '>');
                bool nextIsTagStart = (i < len && html[i] == '<');

                if (prevIsTagEnd && nextIsTagStart) {
                    // Remove whitespace entirely
                } else {
                    output.push_back(' ');
                }
            }
            else
            {
                output.push_back(html[i]);
                i++;
            }
        }
        else if (state == TAG)
        {
             char c = html[i];
             output.push_back(c);
             i++;

             if (c == '>') {
                 if (!sensitiveTagName.empty()) {
                     state = SENSITIVE;
                     // Edge case: self-closing <script /> ?
                     // If the last chars were '/', it is self closing.
                     if (output.size() >= 2 && output[output.size()-2] == '/') {
                         state = CONTENT;
                         sensitiveTagName = "";
                     }
                 } else {
                     state = CONTENT;
                 }
             } else if (c == '"' || c == '\'') {
                 char quote = c;
                 while (i < len) {
                     char qc = html[i];
                     output.push_back(qc);
                     i++;
                     if (qc == quote) {
                         break;
                     }
                 }
             }
        }
        else if (state == SENSITIVE)
        {
            // We look for </tagname>
            // We just copy blindly until we hit it.

             if (html[i] == '<' && i + 1 < len && html[i+1] == '/')
            {
                 bool match = true;
                 size_t checkPos = i + 2;
                 if (checkPos + sensitiveTagName.length() > len) match = false;
                 else {
                     for (size_t j=0; j<sensitiveTagName.length(); ++j) {
                         if (tolower(html[checkPos + j]) != sensitiveTagName[j]) {
                             match = false;
                             break;
                         }
                     }
                 }

                 if (match) {
                     size_t nextC = i + 2 + sensitiveTagName.length();
                     if (nextC < len && (html[nextC] == '>' || isWhitespace(html[nextC]))) {
                         // Found closing tag.
                         // Transition to TAG to parse the closing tag properly (and then to CONTENT)
                         state = TAG;
                         sensitiveTagName = ""; // We are done with this sensitive block
                         output.push_back(html[i]);
                         i++;
                         continue;
                     }
                 }
            }

            output.push_back(html[i]);
            i++;
        }
    }

    // Trim result?
    return output;
}

} // namespace plugin
} // namespace drogon

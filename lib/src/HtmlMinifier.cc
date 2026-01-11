#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/HttpAppFramework.h>
#include <regex>

namespace drogon
{
namespace plugin
{

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    app().registerPreSendingAdvice([](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
        if (resp->getContentType() == CT_TEXT_HTML)
        {
            // Only minify if it looks like HTML
            std::string body(resp->getBody());
            // Basic check to see if it's worth minifying (not empty)
            if (!body.empty())
            {
                 // Perform minification
                 std::string minified = minifyHtml(body);
                 resp->setBody(std::move(minified));
            }
        }
    });
}

void HtmlMinifier::shutdown()
{
}

// A simple state machine based minifier to be safer than regex
// Preserves content in <script>, <style>, <pre>, <textarea>
std::string minifyHtml(const std::string &html)
{
    std::string output;
    output.reserve(html.size());

    enum State {
        DATA,
        TAG_OPEN,
        TAG_NAME,
        IN_TAG,
        ATTRIBUTE_NAME,
        ATTRIBUTE_VALUE_DQ, // double quote
        ATTRIBUTE_VALUE_SQ, // single quote
        COMMENT_START,
        COMMENT,
        RAW_TEXT // For script, style, pre, textarea
    };

    State state = DATA;
    std::string tagName;
    std::string rawEndTag;
    bool isClosingTag = false;

    // Elements to preserve content
    const std::vector<std::string> rawTags = {"script", "style", "pre", "textarea"};

    auto isWhitespace = [](char c) {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    };

    for (size_t i = 0; i < html.size(); ++i)
    {
        char c = html[i];
        char nextC = (i + 1 < html.size()) ? html[i + 1] : '\0';

        switch (state)
        {
            case DATA:
                if (c == '<')
                {
                    if (nextC == '!') {
                         state = COMMENT_START;
                         // Don't push yet, we might want to strip it
                         // For now, let's just push and handle strip in COMMENT state?
                         // Or buffer?
                         // Let's implement comment stripping properly.
                         // Don't push anything until we know it's not a comment we want to remove.
                         // But COMMENT_START includes <!DOCTYPE> which we should keep.
                         output.push_back(c);
                    } else if (nextC == '/') {
                         state = TAG_OPEN;
                         output.push_back(c);
                         isClosingTag = false; // Will set to true in TAG_OPEN
                    } else if (std::isalpha(nextC)) {
                         state = TAG_OPEN;
                         output.push_back(c);
                         isClosingTag = false;
                    } else {
                         output.push_back(c);
                    }
                }
                else if (isWhitespace(c))
                {
                     // Collapse whitespace
                     // If output is empty, don't push leading space
                     if (output.empty()) continue;

                     // If last char is already space, don't push another
                     if (output.back() == ' ') continue;

                     output.push_back(' ');
                }
                else
                {
                    output.push_back(c);
                }
                break;

            case COMMENT_START:
                 output.push_back(c);
                 if (c == '>' && output.size() >= 4 && output.substr(output.size()-3) == "-->") {
                     state = DATA;
                 }
                 // Logic to detect <!--
                 if (output.size() >= 4 && output.substr(output.size()-4) == "<!--") {
                     state = COMMENT;
                     // Remove "<!--" from output to start stripping
                     output.resize(output.size() - 4);
                 }
                 break;

            case COMMENT:
                 // We are in a comment block that we want to remove.
                 // We just wait for "-->"
                 // Since we stripped "<!--", we just ignore chars until we see "-->"
                 // Wait, we need to track the sequence "-->".
                 // Since we are not pushing to output, we can't look back at output.
                 // We need to look ahead or buffer?
                 // Simple lookahead is not enough as we iterate.
                 // But we can check if c is '-' and next chars are '->'.
                 if (c == '-' && i + 2 < html.size() && html[i+1] == '-' && html[i+2] == '>') {
                     // Found end of comment.
                     i += 2; // Skip "->"
                     state = DATA;
                 }
                 break;

            case TAG_OPEN:
                output.push_back(c);
                if (std::isalpha(c)) {
                    state = TAG_NAME;
                    tagName.clear();
                    tagName.push_back(std::tolower(c));
                } else if (c == '/') {
                    isClosingTag = true;
                }
                break;

            case TAG_NAME:
                if (std::isalnum(c) || c == '-') {
                    output.push_back(c);
                    tagName.push_back(std::tolower(c));
                } else {
                    // Tag name finished
                    bool isRaw = false;
                    for(const auto& t : rawTags) {
                         if (tagName == t) {
                             isRaw = true;
                             rawEndTag = "</" + t;
                             break;
                         }
                    }
                    // Crucial fix: Only treat as raw if it is NOT a closing tag
                    if (isClosingTag) {
                        isRaw = false;
                    }

                    state = IN_TAG;
                    i--; // Reprocess the current character in IN_TAG state
                }
                break;

            case IN_TAG:
                output.push_back(c);
                if (c == '>') {
                     // Tag closed.
                     bool isRaw = false;
                     // Re-check raw tag because we might have transitioned from TAG_NAME
                     for(const auto& t : rawTags) {
                         if (tagName == t) {
                             isRaw = true;
                             break;
                         }
                     }

                     if (isClosingTag) isRaw = false;

                     if (isRaw) {
                         state = RAW_TEXT;
                     } else {
                         state = DATA;
                     }
                } else if (c == '"') {
                    state = ATTRIBUTE_VALUE_DQ;
                } else if (c == '\'') {
                    state = ATTRIBUTE_VALUE_SQ;
                }
                break;

            case ATTRIBUTE_VALUE_DQ:
                output.push_back(c);
                if (c == '"') state = IN_TAG;
                break;

            case ATTRIBUTE_VALUE_SQ:
                output.push_back(c);
                if (c == '\'') state = IN_TAG;
                break;

            case RAW_TEXT:
                output.push_back(c);
                // check if we match rawEndTag (case insensitive?)
                // Usually matching exact lower case is enough if we normalized.
                // But html is case insensitive.
                if (output.size() >= rawEndTag.size()) {
                    std::string suffix = output.substr(output.size() - rawEndTag.size());
                    std::string lowerSuffix = suffix;
                    std::transform(lowerSuffix.begin(), lowerSuffix.end(), lowerSuffix.begin(), ::tolower);
                    if (lowerSuffix == rawEndTag) {
                        // Found closing tag start, e.g. "</script"
                        state = TAG_NAME; // Continue parsing the tag
                        tagName = rawEndTag.substr(2); // "script"
                        isClosingTag = true; // It is a closing tag
                    }
                }
                break;
        }
    }

    return output;
}

}  // namespace plugin
}  // namespace drogon

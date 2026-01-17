/**
 *
 *  HtmlMinifier.cc
 *  Anarthal, 2023
 *
 */

#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>
#include <regex>

namespace drogon
{
namespace plugin
{

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    // Register PreSendingAdvice
    drogon::app().registerPreSendingAdvice(
        [](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
            // Check if Content-Type is text/html
            if (resp->contentType() == CT_TEXT_HTML)
            {
                auto body = resp->body(); // string_view
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
    // Simple regex-based minifier.
    std::string output = html;

    // Remove comments
    // Use [\s\S] to match any character including newlines
    static const std::regex comments("<!--([\\s\\S]*?)-->", std::regex_constants::optimize);
    output = std::regex_replace(output, comments, "");

    // Collapse whitespace between tags: > \s+ <  -> ><
    // This captures whitespace that is strictly between tags.
    // WARNING: This is context-insensitive and will collapse whitespace inside <pre> tags
    // or other whitespace-sensitive elements if they contain nested tags with whitespace.
    // Ideally, a proper HTML parser should be used to avoid touching pre/script/style/textarea.

    static const std::regex betweenTags(">\\s+<", std::regex_constants::optimize);
    output = std::regex_replace(output, betweenTags, "><");

    return output;
}

}  // namespace plugin
}  // namespace drogon

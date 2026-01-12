#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/HttpAppFramework.h>
#include <regex>

namespace drogon
{
namespace plugin
{

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    // Register the advice to minify HTML responses
    drogon::app().registerPreSendingAdvice(
        [](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
            if (resp->getContentType() == CT_TEXT_HTML)
            {
                std::string body(resp->getBody().data(), resp->getBody().length());
                std::string minified = HtmlMinifier::minify(body);
                resp->setBody(std::move(minified));
            }
        });
}

void HtmlMinifier::shutdown()
{
}

std::string HtmlMinifier::minify(const std::string &html)
{
    // C++ std::regex doesn't support dotall flag directly.
    // We use [\s\S] to match any character including newlines.
    std::regex comment_regex("<!--[\\s\\S]*?-->");
    std::string no_comments = std::regex_replace(html, comment_regex, "");

    // Remove whitespace between tags ONLY if it contains a newline.
    // This is safer than removing all whitespace which might break inline elements layout.
    // > matches >
    // \s* matches any whitespace (including newlines)
    // [\r\n] matches a newline character (ensures there is at least one)
    // \s* matches trailing whitespace
    // < matches <
    std::regex between_tags(">\\s*[\\r\\n]\\s*<");
    std::string result = std::regex_replace(no_comments, between_tags, "><");

    return result;
}

}
}

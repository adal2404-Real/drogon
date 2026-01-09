/**
 *
 *  @file HtmlMinifier.cc
 *
 *  Drogon
 *
 */

#include <drogon/plugins/HtmlMinifier.h>
#include <regex>

namespace drogon
{
namespace plugin
{

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    // Register the advice
    app().registerPreSendingAdvice([this](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
        minify(resp);
    });
}

void HtmlMinifier::shutdown()
{
}

void HtmlMinifier::minify(const HttpResponsePtr &resp)
{
    if (resp->contentType() == CT_TEXT_HTML)
    {
        std::string body(resp->getBody());

        // Simple minification: remove whitespace between tags
        // Note: This is a very basic minifier and might not handle all edge cases correctly.
        // For a production-ready system, a proper HTML parser/minifier library should be used.
        // We avoid complex regex to prevent ReDoS or breaking scripts/styles.
        // We only collapse whitespace between > and <

        static const std::regex ws_re(">\\s+<");
        body = std::regex_replace(body, ws_re, "><");

        // Also trim leading/trailing whitespace
        size_t first = body.find_first_not_of(" \t\n\r");
        if (std::string::npos == first)
        {
             body = "";
        }
        else
        {
             size_t last = body.find_last_not_of(" \t\n\r");
             body = body.substr(first, (last - first + 1));
        }

        resp->setBody(std::move(body));
    }
}

}  // namespace plugin
}  // namespace drogon

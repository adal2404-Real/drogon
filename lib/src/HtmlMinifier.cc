#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/HttpAppFramework.h>
#include <regex>

namespace drogon
{
namespace plugin
{

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    app().registerPreSendingAdvice([](const HttpRequestPtr &, const HttpResponsePtr &resp) {
        minify(resp);
    });
}

void HtmlMinifier::shutdown()
{
}

void HtmlMinifier::minify(const HttpResponsePtr &resp)
{
    auto contentType = resp->getContentType();
    if (contentType == CT_TEXT_HTML)
    {
        std::string body(std::string(resp->getBody()));
        static const std::regex htmlCommentRegex("<!--[\\s\\S]*?-->");
        std::string result = std::regex_replace(body, htmlCommentRegex, "");
        resp->setBody(std::move(result));
    }
    else if (contentType == CT_TEXT_JAVASCRIPT)
    {
        std::string body(std::string(resp->getBody()));
        static const std::regex jsCommentRegex("/\\*[\\s\\S]*?\\*/");
        std::string result = std::regex_replace(body, jsCommentRegex, "");
        resp->setBody(std::move(result));
    }
}

} // namespace plugin
} // namespace drogon

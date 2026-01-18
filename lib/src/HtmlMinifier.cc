#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/HttpAppFramework.h>
#include <regex>

using namespace drogon;
using namespace drogon::plugin;

void HtmlMinifier::initAndStart(const Json::Value &config)
{
    app().registerPreSendingAdvice([](const HttpRequestPtr &, const HttpResponsePtr &resp) {
        if (resp->getContentType() == CT_TEXT_HTML)
        {
            std::string body(resp->getBody());
            if (!body.empty())
            {
                std::string minified = minify(body);
                resp->setBody(std::move(minified));
            }
        }
    });
}

void HtmlMinifier::shutdown()
{
}

std::string HtmlMinifier::minify(const std::string &html)
{
    std::string output = html;
    // Remove comments
    // Note: This simple regex might match "<!--" inside strings, which is a known limitation
    // of regex-based parsing.
    static const std::regex commentRegex("<!--[\\s\\S]*?-->");
    output = std::regex_replace(output, commentRegex, "");

    // We do not collapse whitespace as it can break inline formatting (e.g. <span>a</span> <span>b</span>)
    // and <pre> blocks. A full HTML parser is required for safe whitespace minification.

    return output;
}

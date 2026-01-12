#pragma once
#include <drogon/plugins/Plugin.h>
#include <string>

namespace drogon
{
namespace plugin
{
class DROGON_EXPORT HtmlMinifier : public drogon::Plugin<HtmlMinifier>
{
public:
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;
    static std::string minify(const std::string &html);
};
}
}

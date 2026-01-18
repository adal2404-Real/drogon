#pragma once

#include <drogon/plugins/Plugin.h>
#include <string>

namespace drogon
{
namespace plugin
{
class HtmlMinifier : public Plugin<HtmlMinifier>
{
  public:
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;
    static std::string minify(const std::string &html);
};
} // namespace plugin
} // namespace drogon

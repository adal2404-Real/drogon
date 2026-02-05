#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/exports.h>
#include <drogon/HttpResponse.h>

namespace drogon
{
namespace plugin
{

class DROGON_EXPORT HtmlMinifier : public Plugin<HtmlMinifier>
{
  public:
    HtmlMinifier() {}
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;

    static void minify(const HttpResponsePtr &resp);
};

} // namespace plugin
} // namespace drogon

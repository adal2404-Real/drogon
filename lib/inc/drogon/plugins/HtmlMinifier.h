/**
 *
 *  @file HtmlMinifier.h
 *
 *  Drogon
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/HttpAppFramework.h>

namespace drogon
{
namespace plugin
{
class DROGON_EXPORT HtmlMinifier : public drogon::Plugin<HtmlMinifier>
{
  public:
    HtmlMinifier() {}
    virtual ~HtmlMinifier() {}

    virtual void initAndStart(const Json::Value &config) override;

    virtual void shutdown() override;

  protected:
    void minify(const HttpResponsePtr &resp);
};

}  // namespace plugin
}  // namespace drogon

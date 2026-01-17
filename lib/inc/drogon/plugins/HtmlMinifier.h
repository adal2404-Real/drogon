/**
 *
 *  HtmlMinifier.h
 *  Anarthal, 2023
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/exports.h>
#include <string>

namespace drogon
{
namespace plugin
{

class DROGON_EXPORT HtmlMinifier : public drogon::Plugin<HtmlMinifier>
{
  public:
    HtmlMinifier() {}
    ~HtmlMinifier() override {}
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;

    // Expose minify logic as static for testing and general usage
    static std::string minify(const std::string &html);
};

}  // namespace plugin
}  // namespace drogon

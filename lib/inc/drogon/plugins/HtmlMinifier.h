/**
 *
 *  @file HtmlMinifier.h
 *  @author Jules
 *
 *  Copyright 2024, Jules.  All rights reserved.
 *  https://github.com/an-tao/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>

namespace drogon
{
namespace plugin
{

class HtmlMinifier : public Plugin<HtmlMinifier>
{
  public:
    HtmlMinifier() {}
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;
};

// Exposed for testing purposes
std::string minifyHtml(const std::string& input);

}  // namespace plugin
}  // namespace drogon

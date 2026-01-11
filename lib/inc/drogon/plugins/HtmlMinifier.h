/**
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
/**
 * @brief The HtmlMinifier plugin minifies HTML responses before sending them to the client.
 *
 * The json configuration is as follows:
 *
 * @code
  {
     "name": "drogon::plugin::HtmlMinifier",
     "dependencies": [],
     "config": {
     }
  }
  @endcode
 *
 */
class DROGON_EXPORT HtmlMinifier : public drogon::Plugin<HtmlMinifier>
{
  public:
    HtmlMinifier() {}
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;
};

// Exposed for testing purposes
DROGON_EXPORT std::string minifyHtml(const std::string &html);

}  // namespace plugin
}  // namespace drogon

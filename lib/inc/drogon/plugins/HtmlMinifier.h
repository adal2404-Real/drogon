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
#include <string>

namespace drogon
{
namespace plugin
{

class DROGON_EXPORT HtmlMinifier : public drogon::Plugin<HtmlMinifier>
{
  public:
    HtmlMinifier() {}
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;

    /**
     * @brief Minify HTML content.
     * Exposed for testing purposes.
     *
     * @param html The HTML string to minify.
     * @return std::string The minified HTML.
     */
    static std::string minify(const std::string &html);
};

} // namespace plugin
} // namespace drogon

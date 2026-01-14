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
#include <regex>

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
     * @brief Minifies the HTML content.
     * Exposed as public static for testing purposes.
     *
     * @param content The HTML content to minify.
     * @return std::string The minified HTML content.
     */
    static std::string minify(const std::string &content);

  private:
    static std::regex whitespaceRegex_;
    static std::regex commentRegex_;
};

}  // namespace plugin
}  // namespace drogon

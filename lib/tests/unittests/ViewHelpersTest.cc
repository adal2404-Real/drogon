#include <drogon/utils/ViewHelpers.h>
#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>
#include <string>

using namespace drogon::utils;
using namespace drogon::plugin;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    trantor::Date d = trantor::Date::now();
    std::string fmt = "%Y-%m-%d";
    std::string res = ViewHelpers::formatDate(d, fmt);
    CHECK(res.size() > 0);

    // Test escapeHtml
    CHECK(ViewHelpers::escapeHtml("<div>") == "&lt;div&gt;");
    CHECK(ViewHelpers::escapeHtml("\"quote\"") == "&quot;quote&quot;");
    CHECK(ViewHelpers::escapeHtml("'apos'") == "&apos;apos&apos;");
    CHECK(ViewHelpers::escapeHtml("a & b") == "a &amp; b");

    // Test generateCsrfToken
    std::string token = ViewHelpers::generateCsrfToken();
    CHECK(token.size() > 0);
    CHECK(token != ViewHelpers::generateCsrfToken());

    // Test HtmlMinifier
    std::string html = "<div>\n  <span>Text</span>\n</div><!-- comment -->";
    // "<!-- comment -->" removed. Whitespace preserved.
    std::string minified = HtmlMinifier::minify(html);
    CHECK(minified == "<div>\n  <span>Text</span>\n</div>");

    std::string html3 = "<div>   </div>";
    CHECK(HtmlMinifier::minify(html3) == "<div>   </div>");
}

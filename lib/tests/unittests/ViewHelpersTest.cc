#include <drogon/utils/ViewHelpers.h>
#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>
#include <iostream>

using namespace drogon::utils;
using namespace drogon::plugin;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    auto now = trantor::Date::now();
    std::string formatted = ViewHelpers::formatDate(now, "%Y-%m-%d");
    CHECK(formatted.length() == 10);
    CHECK(formatted[4] == '-');
    CHECK(formatted[7] == '-');

    // Test generateCsrfToken
    std::string token1 = ViewHelpers::generateCsrfToken();
    std::string token2 = ViewHelpers::generateCsrfToken();
    CHECK(!token1.empty());
    CHECK(token1 != token2);

    // Test escapeHtml
    std::string input = "<script>alert('XSS')</script> & \"";
    std::string escaped = ViewHelpers::escapeHtml(input);
    CHECK(escaped.find("<") == std::string::npos);
    CHECK(escaped.find(">") == std::string::npos);
    CHECK(escaped.find("&lt;") != std::string::npos);
    CHECK(escaped.find("&gt;") != std::string::npos);
    CHECK(escaped.find("&amp;") != std::string::npos);
    CHECK(escaped.find("&quot;") != std::string::npos);
}

DROGON_TEST(HtmlMinifierTest)
{
    // Basic minification
    std::string html = "<div> \n  <p>  Hello  </p> \n </div>";
    std::string minified = HtmlMinifier::minify(html);
    // Expect: "<div><p> Hello </p></div>" -> We decided to replace whitespace with ' ' if not between > <
    // Actually, "  Hello  " -> " Hello "
    // "<div> \n  <p>" -> "<div><p>" (whitespace between tags removed)
    // "<p>  Hello  </p>" -> "<p> Hello </p>" (internal whitespace collapsed)
    // "</p> \n </div>" -> "</p></div>"
    CHECK(minified == "<div><p> Hello </p></div>");

    // Sensitive tag preservation
    std::string sensitive = "<div> <script> \n var x = ' > < '; \n </script> </div>";
    std::string min_sensitive = HtmlMinifier::minify(sensitive);
    // "<div> <script>" -> "<div><script>"
    // Inside script: " \n var x = ' > < '; \n " should be preserved exactly?
    // Wait, my implementation preserves EVERYTHING in SENSITIVE mode.
    // So " \n var x = ' > < '; \n " remains.
    // Result: "<div><script> \n var x = ' > < '; \n </script></div>"

    // Check that ' > < ' is NOT collapsed.
    CHECK(min_sensitive.find("' > < '") != std::string::npos);
    // Check that surrounding whitespace of script tag is removed/collapsed
    CHECK(min_sensitive.find("<div><script>") != std::string::npos);
    CHECK(min_sensitive.find("</script></div>") != std::string::npos);

    // Pre tag
    std::string pre = "<pre>  \n  Line 1  \n  Line 2  </pre>";
    std::string min_pre = HtmlMinifier::minify(pre);
    CHECK(min_pre == pre); // Should be exactly same because content is whitespace + text

    // Mixed
    std::string mixed = "<div> <span> A </span> </div>";
    CHECK(HtmlMinifier::minify(mixed) == "<div><span> A </span></div>");
}

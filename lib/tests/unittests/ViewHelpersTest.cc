#include <drogon/utils/ViewHelpers.h>
#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>
#include <trantor/utils/Date.h>
#include <iostream>

using namespace drogon;
using namespace drogon::utils;
using namespace drogon::plugin;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    {
        // Use a fixed date: 2023-01-01 12:00:00 UTC
        // Note: formatDate uses local time via trantor.
        // So we can't easily assert exact string without knowing timezone,
        // unless we mock timezone or just check format structure.
        // Or we use trantor's Date now.
        auto now = trantor::Date::now();
        std::string fmt = "%Y-%m-%d";
        std::string res = ViewHelpers::formatDate(now, fmt);
        // Basic check length and format
        CHECK(res.size() == 10);
        CHECK(res[4] == '-');
        CHECK(res[7] == '-');
    }

    // Test generateCsrfToken
    {
        std::string token1 = ViewHelpers::generateCsrfToken();
        std::string token2 = ViewHelpers::generateCsrfToken();
        CHECK(!token1.empty());
        CHECK(token1 != token2);
    }

    // Test escapeHtml
    {
        std::string raw = "<script>alert('xss')</script>";
        std::string escaped = ViewHelpers::escapeHtml(raw);
        CHECK(escaped == "&lt;script&gt;alert(&apos;xss&apos;)&lt;/script&gt;");

        std::string raw2 = "\" & '";
        std::string escaped2 = ViewHelpers::escapeHtml(raw2);
        CHECK(escaped2 == "&quot; &amp; &apos;");
    }
}

DROGON_TEST(HtmlMinifierTest)
{
    // Test HtmlMinifier::minify static method
    {
        std::string input = "<div>  <span>Hello</span>  </div>";
        // Expected: <div><span>Hello</span></div>
        // Assuming implementation replaces >\s+< with ><
        std::string expected = "<div><span>Hello</span></div>";
        std::string result = HtmlMinifier::minify(input);
        CHECK(result == expected);
    }

    {
        std::string input = "<!-- comment --><div>Text</div>";
        std::string expected = "<div>Text</div>";
        std::string result = HtmlMinifier::minify(input);
        CHECK(result == expected);
    }

    {
        // Multi-line comment test
        std::string input = "<div><!-- \n multi \n line \n comment -->Text</div>";
        std::string expected = "<div>Text</div>";
        std::string result = HtmlMinifier::minify(input);
        CHECK(result == expected);
    }

    {
        std::string input = "<div>\n  <p>\n    Content\n  </p>\n</div>";
        // >\n  < -> ><
        // >\n    C -> >\n    C (not matched by >\s+< because Content starts)
        // Actually >\s+< matches strictly between > and <.
        // "<div>\n  <p>" -> "<div><p>"
        // "<p>\n    Content" -> not matched.
        // "Content\n  </p>" -> not matched.
        // "</p>\n</div>" -> "</p></div>"

        std::string expected = "<div><p>\n    Content\n  </p></div>";
        std::string result = HtmlMinifier::minify(input);
        CHECK(result == expected);
    }
}

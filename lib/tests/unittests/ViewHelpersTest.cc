#include <drogon/utils/ViewHelpers.h>
#include <drogon/drogon_test.h>
#include <drogon/plugins/HtmlMinifier.h>

using namespace drogon::utils;
using namespace drogon::plugin;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    trantor::Date date(1678886400000000); // 2023-03-15 16:00:00 UTC
    std::string formatted = ViewHelpers::formatDate(date, "%Y-%m-%d");
    CHECK(formatted == "2023-03-15");

    // Test generateCsrfToken
    std::string token1 = ViewHelpers::generateCsrfToken();
    std::string token2 = ViewHelpers::generateCsrfToken();
    CHECK(!token1.empty());
    CHECK(token1 != token2);

    // Test escapeHtml
    std::string html = "<script>alert('XSS')</script> & \"";
    std::string escaped = ViewHelpers::escapeHtml(html);
    CHECK(escaped == "&lt;script&gt;alert(&apos;XSS&apos;)&lt;/script&gt; &amp; &quot;");
}

DROGON_TEST(HtmlMinifierTest)
{
    // Test basic minification
    std::string input = "<div>\n  <p>Hello</p>\n</div><!-- comment -->";
    // We expect comments to be removed and whitespace between tags to be collapsed
    // "<div>" -> "<div>"
    // "\n  " -> followed by <p>, so skipped
    // "<p>Hello</p>" -> "<p>Hello</p>"
    // "\n" -> followed by </div>, so skipped
    // "</div>" -> "</div>"
    std::string expected = "<div><p>Hello</p></div>";
    CHECK(HtmlMinifier::minify(input) == expected);

    // Test preserving <pre>
    std::string preInput = "<div>\n<pre>\n  Code block  \n</pre>\n</div>";
    std::string preExpected = "<div><pre>\n  Code block  \n</pre></div>";
    CHECK(HtmlMinifier::minify(preInput) == preExpected);

    // Test preserving <script>
    std::string scriptInput = "<script>\n  if (a < b) { alert('hello'); }\n</script>";
    std::string scriptExpected = "<script>\n  if (a < b) { alert('hello'); }\n</script>";
    CHECK(HtmlMinifier::minify(scriptInput) == scriptExpected);

    // Test dangerous script content (the blocker case)
    std::string dangerousScript = "<script> if (a > \n < b) </script>";
    CHECK(HtmlMinifier::minify(dangerousScript) == dangerousScript);

    // Test preserving <textarea>
    std::string areaInput = "<textarea>  Default   Text  </textarea>";
    CHECK(HtmlMinifier::minify(areaInput) == areaInput);

    // Test mixed content
    std::string mixed = "<div> <p> A </p> </div>";
    // > < detection:
    // <div>_ -> space followed by <p>, should be removed. -> <div><p>
    // <p>_A -> space followed by A. Preserved. -> <p> A
    // A_</p> -> A followed by space then <. Space removed?
    // Wait, my logic only checks whitespace AFTER '>'.
    // It does NOT check whitespace BEFORE '<' unless it was also after '>'.
    // " <p> A </p> "
    // ">" (from div) followed by " <" -> remove space -> "<div><p>"
    // "<p>" ... " A " ... "</p>"
    // ">" (from /p) followed by " <" -> remove space -> "</div>"
    // So "<div><p> A </p></div>"
    std::string mixedExpected = "<div><p> A </p></div>";
    CHECK(HtmlMinifier::minify(mixed) == mixedExpected);
}

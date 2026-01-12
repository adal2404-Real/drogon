#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>
#include <string>

using namespace drogon::plugin;

DROGON_TEST(HtmlMinifierTest)
{
    std::string html = R"(
    <html>
        <!-- This is a comment -->
        <body>
            <div>
                Content
            </div>
            <span>A</span> <span>B</span>
        </body>
    </html>
    )";

    std::string minified = HtmlMinifier::minify(html);

    // Check if comment is removed
    CHECK(minified.find("<!--") == std::string::npos);

    // Check if whitespace between tags with newlines is removed
    CHECK(minified.find("<html><body>") != std::string::npos);
    CHECK(minified.find("<body><div>") != std::string::npos);

    // Check if inline space is preserved
    // "<span>A</span> <span>B</span>" should remain as is (or at least space preserved)
    // In the input string, there is indentation before <span>A</span> and after <span>B</span> (newlines).
    // The space BETWEEN </span> and <span> is a space, not a newline.
    // So it should be preserved.
    CHECK(minified.find("<span>A</span> <span>B</span>") != std::string::npos);

    // Check if surrounding newlines were removed
    // The newline after </div> and before <span> should be removed.
    CHECK(minified.find("</div><span>") != std::string::npos);

    // The newline after <span>B</span> and before </body> should be removed.
    CHECK(minified.find("</span></body>") != std::string::npos);

    // The newline after </body> and before </html> should be removed.
    CHECK(minified.find("</body></html>") != std::string::npos);
}

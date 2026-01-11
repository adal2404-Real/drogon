#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>

using namespace drogon::plugin;

DROGON_TEST(HtmlMinifierTest)
{
    // Test basic minification
    {
        std::string html = "<html>\n  <body>\n    <h1>Hello</h1>\n  </body>\n</html>";
        // The minifier collapses whitespace.
        // "<html>"
        // "\n  " -> " "
        // "<body>"
        // ...
        // Expected: "<html> <body> <h1>Hello</h1> </body> </html>"
        std::string minified = minifyHtml(html);
        CHECK(minified == "<html> <body> <h1>Hello</h1> </body> </html>");
    }

    // Test script preservation
    {
        std::string html = "<script>\n  console.log('hello');\n</script>";
        std::string minified = minifyHtml(html);
        CHECK(minified == html);
    }

    // Test that process continues correctly AFTER a script tag
    {
        std::string html = "<script>var x=1;</script><div>\n content \n</div>";
        std::string minified = minifyHtml(html);
        // "<script>var x=1;</script>" preserved.
        // "<div>"
        // "\n content \n" -> " content "
        // "</div>"
        CHECK(minified == "<script>var x=1;</script><div> content </div>");
    }

    // Test complex nesting
    {
        std::string html = "<div>\n<span> text </span>\n</div>";
        std::string minified = minifyHtml(html);
        CHECK(minified == "<div> <span> text </span> </div>");
    }

    // Test case insensitive closing tag
    {
        std::string html = "<SCRIPT> var x=1; </SCRIPT>";
        std::string minified = minifyHtml(html);
        CHECK(minified == html);
    }

    // Test comment stripping
    {
        std::string html = "<div><!-- This is a comment --></div>";
        std::string minified = minifyHtml(html);
        CHECK(minified == "<div></div>");

        std::string html2 = "<!-- comment --><span>text</span>";
        CHECK(minifyHtml(html2) == "<span>text</span>");
    }

    // Test not stripping !DOCTYPE or others
    {
        std::string html = "<!DOCTYPE html><html></html>";
        // space might be inserted between html and html because of collapse?
        // "<!DOCTYPE html>" -> ends with >.
        // "<html>" -> starts with <.
        // No space between them in original.
        CHECK(minifyHtml(html) == "<!DOCTYPE html><html></html>");
    }
}

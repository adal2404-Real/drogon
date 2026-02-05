#include <drogon/utils/ViewHelpers.h>
#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>
#include <drogon/HttpResponse.h>

using namespace drogon;
using namespace drogon::utils;
using namespace drogon::plugin;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    auto now = trantor::Date::now();
    // Verify it doesn't crash and returns non-empty string
    // Specific format validation depends on implementation of toCustomFormattedStringLocal
    std::string formatted = ViewHelpers::formatDate(now, "%Y-%m-%d");
    CHECK(formatted.size() > 0);

    // Test escapeHtml
    std::string raw = "<script>alert('xss');</script>";
    std::string escaped = ViewHelpers::escapeHtml(raw);
    CHECK(escaped == "&lt;script&gt;alert(&apos;xss&apos;);&lt;/script&gt;");

    std::string quote = " \" ' ";
    std::string escapedQuote = ViewHelpers::escapeHtml(quote);
    CHECK(escapedQuote == " &quot; &apos; ");

    // Test generateCsrfToken
    std::string token1 = ViewHelpers::generateCsrfToken();
    std::string token2 = ViewHelpers::generateCsrfToken();
    CHECK(!token1.empty());
    CHECK(token1 != token2);
}

DROGON_TEST(HtmlMinifierTest)
{
    // Test HTML minify
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("<html><!-- comment --><body>Hello</body></html>");
    resp->setContentTypeCode(CT_TEXT_HTML);

    HtmlMinifier::minify(resp);

    CHECK(std::string(resp->getBody()) == "<html><body>Hello</body></html>");

    // Test ignore other content types (e.g. TEXT_PLAIN)
    auto resp2 = HttpResponse::newHttpResponse();
    resp2->setBody("<!-- comment -->");
    resp2->setContentTypeCode(CT_TEXT_PLAIN);
    HtmlMinifier::minify(resp2);
    CHECK(std::string(resp2->getBody()) == "<!-- comment -->");

    // Test multiline HTML comment
    auto resp3 = HttpResponse::newHttpResponse();
    resp3->setBody("<html><!-- \n multi \n line \n comment --><body>Hello</body></html>");
    resp3->setContentTypeCode(CT_TEXT_HTML);
    HtmlMinifier::minify(resp3);
    CHECK(std::string(resp3->getBody()) == "<html><body>Hello</body></html>");

    // Test JS minify
    auto resp4 = HttpResponse::newHttpResponse();
    resp4->setBody("var x = 1; /* comment */ var y = 2;");
    resp4->setContentTypeCode(CT_TEXT_JAVASCRIPT);
    HtmlMinifier::minify(resp4);
    CHECK(std::string(resp4->getBody()) == "var x = 1;  var y = 2;");

    // Test multiline JS comment
    auto resp5 = HttpResponse::newHttpResponse();
    resp5->setBody("var x = 1; /* \n comment \n */ var y = 2;");
    resp5->setContentTypeCode(CT_TEXT_JAVASCRIPT);
    HtmlMinifier::minify(resp5);
    CHECK(std::string(resp5->getBody()) == "var x = 1;  var y = 2;");
}

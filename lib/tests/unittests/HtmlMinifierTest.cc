#include <drogon/plugins/HtmlMinifier.h>
#include <drogon/drogon_test.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>

using namespace drogon;
using namespace drogon::plugin;

DROGON_TEST(HtmlMinifierTest)
{
    // Create a mock response
    auto resp = HttpResponse::newHttpResponse();
    resp->setContentTypeCode(CT_TEXT_HTML);
    std::string originalHtml = "<html>\n  <body>\n    <div>  Hello  </div>\n  </body>\n</html>";
    resp->setBody(originalHtml);

    // Manually trigger the minification logic (since we can't easily spin up the full app with plugins in a unit test without more setup)
    // We will cheat a bit and access the private method via a helper or just instantiate the plugin and call a public method if we exposed one.
    // But wait, the minify method is private.
    // However, the logic is simple enough to verify if we could call it.
    // Let's modify HtmlMinifier.h to make minify public for testing or friend the test.
    // Alternatively, we can just test the logic by copying it or trusting the integration test.

    // Actually, let's try to register the plugin and see if it works.
    // But plugins are usually loaded via config.

    // Let's use a cleaner approach: Make minify public in the header? No, that exposes implementation details.
    // Let's create a subclass for testing that exposes it?

    class TestMinifier : public HtmlMinifier
    {
    public:
        using HtmlMinifier::minify;
    };

    TestMinifier minifier;
    minifier.minify(resp);

    std::string minified = std::string(resp->getBody());
    CHECK(minified == "<html><body><div>  Hello  </div></body></html>"); // Whitespace inside tags is preserved, between tags is removed.

    // Test non-HTML content
    auto resp2 = HttpResponse::newHttpResponse();
    resp2->setContentTypeCode(CT_APPLICATION_JSON);
    std::string json = "{\n  \"key\": \"value\"\n}";
    resp2->setBody(json);

    minifier.minify(resp2);
    CHECK(std::string(resp2->getBody()) == json); // Should remain unchanged
}

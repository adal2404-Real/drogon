#include <drogon/utils/ViewHelpers.h>
#include <drogon/drogon_test.h>
#include <iostream>

using namespace drogon::utils;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    trantor::Date now = trantor::Date::now();
    std::string formatted = ViewHelpers::formatDate(now, "%Y-%m-%d");
    // Just check if it's not empty and has correct length
    CHECK(formatted.length() == 10);

    // Test generateCsrfToken
    std::string token = ViewHelpers::generateCsrfToken();
    CHECK(!token.empty());

    // Test escapeHtml
    std::string html = "<div>\"Hello\" & 'World'</div>";
    std::string escaped = ViewHelpers::escapeHtml(html);
    CHECK(escaped == "&lt;div&gt;&quot;Hello&quot; &amp; &#39;World&#39;&lt;/div&gt;");
}

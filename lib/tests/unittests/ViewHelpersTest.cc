#include <drogon/utils/ViewHelpers.h>
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

using namespace drogon;
using namespace drogon::utils;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    trantor::Date now = trantor::Date::now();
    std::string formatted = ViewHelpers::formatDate(now, "%Y-%m-%d");
    CHECK(formatted.length() == 10);

    // Test escapeHtml
    std::string dangerous = "<script>alert('xss')</script>";
    std::string safe = ViewHelpers::escapeHtml(dangerous);
    CHECK(safe == "&lt;script&gt;alert('xss')&lt;/script&gt;");

    // Test generateCsrfToken
    std::string token1 = ViewHelpers::generateCsrfToken();
    std::string token2 = ViewHelpers::generateCsrfToken();
    CHECK(!token1.empty());
    CHECK(token1 != token2);
}

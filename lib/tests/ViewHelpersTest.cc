#include <drogon/utils/ViewHelpers.h>
#include <drogon/drogon_test.h>
#include <iostream>

using namespace drogon::utils;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    {
        // trantor::Date uses microsecond resolution
        // 1672531200 is 2023-01-01 00:00:00 UTC
        trantor::Date date(1672531200000000LL);
        std::string formatted = ViewHelpers::formatDate(date, "%Y-%m-%d");
        CHECK(formatted == "2023-01-01");
    }

    // Test generateCsrfToken
    {
        std::string token1 = ViewHelpers::generateCsrfToken();
        std::string token2 = ViewHelpers::generateCsrfToken();
        CHECK(token1.length() > 0);
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

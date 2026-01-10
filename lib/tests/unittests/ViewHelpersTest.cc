#include <drogon/utils/ViewHelpers.h>
#include <drogon/drogon_test.h>
#include <iostream>

using namespace drogon::utils;

DROGON_TEST(ViewHelpersTest)
{
    // Test formatDate
    {
        trantor::Date date(2024, 1, 1, 12, 0, 0);
        std::string formatted = ViewHelpers::formatDate(date, "%Y-%m-%d %H:%M:%S");
        CHECK(!formatted.empty());
        CHECK(formatted.find("2024") != std::string::npos);
    }

    // Test generateCsrfToken
    {
        std::string token1 = ViewHelpers::generateCsrfToken();
        std::string token2 = ViewHelpers::generateCsrfToken();
        CHECK(!token1.empty());
        CHECK(token1.length() == 32);
        CHECK(token1 != token2);
    }

    // Test escapeHtml
    {
        std::string raw = "<script>alert('xss');</script>";
        std::string escaped = ViewHelpers::escapeHtml(raw);
        CHECK(escaped == "&lt;script&gt;alert(&apos;xss&apos;);&lt;/script&gt;");

        std::string raw2 = "\"quote\" & ampersand";
        std::string escaped2 = ViewHelpers::escapeHtml(raw2);
        CHECK(escaped2 == "&quot;quote&quot; &amp; ampersand");
    }
}

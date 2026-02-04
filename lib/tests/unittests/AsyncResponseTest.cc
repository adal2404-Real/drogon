#include <drogon/drogon_test.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpAppFramework.h>

using namespace drogon;

DROGON_TEST(AsyncResponseTest)
{
    auto req = HttpRequest::newHttpRequest();

    SUBSECTION(SetAndRetrieve)
    {
        bool called = false;
        req->setResponseCallback([&called](const HttpResponsePtr &){
            called = true;
        });

        auto asyncFunc = req->newAsyncLater();
        // Since newAsyncLater returns a copy of std::function, it should be callable.
        // It might be empty if we didn't set it (but we did).

        if (asyncFunc)
        {
            asyncFunc(HttpResponse::newHttpResponse());
            CHECK(called == true);
        }
        else
        {
            FAIL("asyncFunc is empty");
        }
    }

    SUBSECTION(EmptyBeforeSet)
    {
        auto req2 = HttpRequest::newHttpRequest();
        auto asyncFunc = req2->newAsyncLater();
        // It should return a valid function (no-op)
        asyncFunc(HttpResponse::newHttpResponse());
    }

    SUBSECTION(SendResponse)
    {
        bool called = false;
        req->setResponseCallback([&called](const HttpResponsePtr &) {
            called = true;
        });
        req->sendResponse(HttpResponse::newHttpResponse());
        CHECK(called == true);
    }
}

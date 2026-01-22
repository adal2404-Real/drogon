#include <drogon/HttpController.h>
#include <drogon/HttpFilter.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/drogon_test.h>
#include <drogon/Attribute.h>
#include "../../src/HttpControllersRouter.h"
#include "../../src/HttpRequestImpl.h"

using namespace drogon;

class AnnotationTestController : public drogon::HttpController<AnnotationTestController>
{
  public:
    METHOD_LIST_BEGIN
    // Test basic annotation
    METHOD_ADD(AnnotationTestController::basic, "/basic", ROLES_ALLOWED("User"));
    // Test generic annotation
    METHOD_ADD(AnnotationTestController::custom, "/custom", internal::HttpConstraint("custom::key", "custom_value"));
    METHOD_LIST_END

    void basic(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback)
    {
        // Implementation doesn't matter for routing test
        callback(HttpResponse::newHttpResponse());
    }

    void custom(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback)
    {
        callback(HttpResponse::newHttpResponse());
    }
};

class AuthFilter : public HttpFilter<AuthFilter>
{
  public:
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override
    {
        fccb();
    }
};

DROGON_TEST(AnnotationTest_Basic)
{
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    // Path should be /annotationtestcontroller/basic because class is in global namespace
    req->setPath("/annotationtestcontroller/basic");

    auto reqImpl = std::dynamic_pointer_cast<HttpRequestImpl>(req);
    auto result = HttpControllersRouter::instance().route(reqImpl);

    CHECK(result.result == RouteResult::Success);
    if(result.binderPtr)
    {
        // Check if AuthFilter was added
        bool hasAuthFilter = false;
        for(const auto& mw : result.binderPtr->middlewareNames_)
        {
            if(mw == "AuthFilter") hasAuthFilter = true;
        }
        CHECK(hasAuthFilter == true);

        // Check if annotations were injected into request
        auto &attributes = req->attributes();
        CHECK(attributes->find("drogon::annotations") == true);
        if(attributes->find("drogon::annotations"))
        {
            auto annotations = attributes->get<std::map<std::string, std::string>>("drogon::annotations");
            CHECK(annotations.count("drogon::auth") == 1);
            if(annotations.count("drogon::auth"))
            {
                CHECK(annotations.at("drogon::auth") == "User");
            }
        }
    }
}

DROGON_TEST(AnnotationTest_Custom)
{
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    req->setPath("/annotationtestcontroller/custom");

    auto reqImpl = std::dynamic_pointer_cast<HttpRequestImpl>(req);
    auto result = HttpControllersRouter::instance().route(reqImpl);

    CHECK(result.result == RouteResult::Success);
    if(result.binderPtr)
    {
        // Check if annotations were injected into request
        auto &attributes = req->attributes();
        CHECK(attributes->find("drogon::annotations") == true);
        if(attributes->find("drogon::annotations"))
        {
            auto annotations = attributes->get<std::map<std::string, std::string>>("drogon::annotations");
            CHECK(annotations.count("custom::key") == 1);
            if(annotations.count("custom::key"))
            {
                CHECK(annotations.at("custom::key") == "custom_value");
            }
        }
    }
}

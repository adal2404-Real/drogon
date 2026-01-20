#include <drogon/HttpAppFramework.h>
#include <drogon/HttpController.h>
#include <drogon/HttpFilter.h>
#include <drogon/drogon_test.h>
#include "../../src/HttpControllersRouter.h"
#include "../../src/HttpRequestImpl.h"

using namespace drogon;

class AuthFilter : public HttpFilter<AuthFilter>
{
  public:
    void doFilter(const HttpRequestPtr &,
                  FilterCallback &&,
                  FilterChainCallback &&fccb) override
    {
        fccb();
    }
};

DROGON_TEST(AnnotationTest)
{
    // Register a handler with annotation
    drogon::app().registerHandler(
        "/annotation_test",
        [](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            callback(HttpResponse::newHttpResponse());
        },
        {ROLES_ALLOWED("User")}
    );

    // Create a request
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/annotation_test");
    req->setMethod(Get);
    auto reqImpl = std::dynamic_pointer_cast<HttpRequestImpl>(req);

    // Route
    auto result = HttpControllersRouter::instance().route(reqImpl);

    // Verify routing success
    if (result.result != RouteResult::Success)
    {
        FAIL("Routing failed");
    }

    // Verify Auto-applied filter
    bool authFilterFound = false;
    for(const auto& mw : result.binderPtr->middlewareNames_) {
        if(mw == "AuthFilter") {
            authFilterFound = true;
            break;
        }
    }
    REQUIRE(authFilterFound == true);

    // Verify Annotations in Binder
    REQUIRE(result.binderPtr->handlerAnnotations_.size() == 1);
    REQUIRE(result.binderPtr->handlerAnnotations_["drogon::auth"] == "User");

    // Verify Injection into Request
    auto attributes = req->getAttributes();
    REQUIRE(attributes->find("drogon::annotations") == true);
    auto annotations = attributes->get<std::map<std::string, std::string>>("drogon::annotations");
    REQUIRE(annotations.size() == 1);
    REQUIRE(annotations["drogon::auth"] == "User");
}

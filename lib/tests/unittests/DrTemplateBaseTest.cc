#include <drogon/DrTemplateBase.h>
#include <drogon/DrClassMap.h>
#include <drogon/drogon_test.h>
#include <string>
#include <iostream>

using namespace drogon;

// Mock classes
namespace Admin {
namespace Users {
class List : public DrTemplateBase {
public:
    virtual std::string genText(const DrTemplateData &) override { return "Namespace: Admin::Users::List"; }
};
}
}

class Admin_Users_List : public DrTemplateBase {
public:
    virtual std::string genText(const DrTemplateData &) override { return "Flat: Admin_Users_List"; }
};

DROGON_TEST(DrTemplateBaseResolverTest)
{
    // Register mock classes
    DrClassMap::registerClass("Admin::Users::List", []() -> DrObjectBase* { return new Admin::Users::List; });
    DrClassMap::registerClass("Admin_Users_List", []() -> DrObjectBase* { return new Admin_Users_List; });

    // Test 1: Resolve Namespace with extension
    auto t1 = DrTemplateBase::newTemplate("Admin/Users/List.csp");
    CHECK(t1 != nullptr);
    if(t1) CHECK(t1->genText({}) == "Namespace: Admin::Users::List");

    // Test 2: Resolve Namespace without extension
    auto t2 = DrTemplateBase::newTemplate("Admin/Users/List");
    CHECK(t2 != nullptr);
    if(t2) CHECK(t2->genText({}) == "Namespace: Admin::Users::List");

    // Test 3: Resolve Flat Underscore with extension
    // To ensure we are hitting the flat resolution, we use a path that maps to the flat class name
    // Admin/Users/List.csp maps to Admin::Users::List first, so we need to rely on the fact that
    // Admin::Users::List exists.
    // Let's register a unique flat one for explicit testing.
    class FlatOnly : public DrTemplateBase {
    public:
         virtual std::string genText(const DrTemplateData &) override { return "FlatOnly"; }
    };
    DrClassMap::registerClass("Flat_Only_View", []() -> DrObjectBase* { return new FlatOnly; });

    auto t3 = DrTemplateBase::newTemplate("Flat/Only/View.csp");
    CHECK(t3 != nullptr);
    if(t3) CHECK(t3->genText({}) == "FlatOnly");

    // Test 4: Resolve Flat Underscore without extension
    auto t4 = DrTemplateBase::newTemplate("Flat/Only/View");
    CHECK(t4 != nullptr);
    if(t4) CHECK(t4->genText({}) == "FlatOnly");
}

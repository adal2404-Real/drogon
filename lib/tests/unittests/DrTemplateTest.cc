#include <drogon/DrTemplate.h>
#include <drogon/drogon_test.h>
#include <memory>
#include <iostream>

using namespace drogon;

// Mock template class for testing
namespace views
{
namespace Admin
{
namespace Users
{
class List : public DrTemplate<List>
{
  public:
    virtual std::string genText(const DrTemplateData &) override
    {
        return "List View";
    }
};
}  // namespace Users
}  // namespace Admin
}  // namespace views

DROGON_TEST(DrTemplatePathResolutionTest)
{
    // Force instantiation/registration
    // This is sometimes needed in tests where the class isn't otherwise referenced
    // and the linker might discard it or static init doesn't fire.
    // Accessing a static member or instantiating it usually helps.
    std::cout << "Class name: " << views::Admin::Users::List::classTypeName() << std::endl;

    // Debug: print all registered classes
    auto names = DrClassMap::getAllClassName();
    bool found = false;
    for(const auto& name : names) {
        if(name == "views::Admin::Users::List") {
            found = true;
            break;
        }
    }

    if(!found) {
        std::cout << "Available classes:" << std::endl;
        for(const auto& name : names) {
            std::cout << name << std::endl;
        }
    }

    // Test direct class name access
    auto templ1 = DrTemplateBase::newTemplate("views::Admin::Users::List");
    CHECK(templ1 != nullptr);

    // Test path with .csp extension
    auto templ2 = DrTemplateBase::newTemplate("views/Admin/Users/List.csp");
    CHECK(templ2 != nullptr);

    // Test path without extension (New feature)
    auto templ3 = DrTemplateBase::newTemplate("views/Admin/Users/List");
    CHECK(templ3 != nullptr);

    // Test with leading slash
    auto templ4 = DrTemplateBase::newTemplate("/views/Admin/Users/List");
    CHECK(templ4 != nullptr);

    // Test with leading ./
    auto templ5 = DrTemplateBase::newTemplate("./views/Admin/Users/List");
    CHECK(templ5 != nullptr);

    // Test invalid path
    auto templ6 = DrTemplateBase::newTemplate("views/Admin/Users/NonExistent");
    CHECK(templ6 == nullptr);

    // Test invalid class
    auto templ7 = DrTemplateBase::newTemplate("views::Admin::Users::NonExistent");
    CHECK(templ7 == nullptr);
}

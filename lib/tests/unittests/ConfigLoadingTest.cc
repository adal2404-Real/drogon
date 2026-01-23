#include <drogon/HttpAppFramework.h>
#include <drogon/drogon_test.h>
#include <json/json.h>
#include <fstream>
#include <filesystem>

using namespace drogon;

DROGON_TEST(ConfigLoadingTest)
{
    // Test loadConfigJson
    Json::Value config;
    config["custom_config"]["test_key"] = "json_value";

    app().loadConfigJson(config);

    CHECK(app().getCustomConfig()["test_key"].asString() == "json_value");

    // Test loadConfigFile
    // Create a temporary config file
    std::string configContent = R"({
        "custom_config": {
            "test_key": "file_value"
        }
    })";

    std::string filename = "temp_config_test.json";
    {
        std::ofstream configFile(filename);
        configFile << configContent;
    }

    app().loadConfigFile(filename);

    CHECK(app().getCustomConfig()["test_key"].asString() == "file_value");

    // Cleanup
    std::filesystem::remove(filename);
}

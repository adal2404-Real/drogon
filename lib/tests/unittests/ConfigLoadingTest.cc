#include <drogon/drogon_test.h>
#include <drogon/HttpAppFramework.h>
#include <fstream>
#include <filesystem>

using namespace drogon;

DROGON_TEST(ConfigLoadingTest)
{
    // Test loadConfigJson
    {
        Json::Value config;
        config["app"]["threads_num"] = 10;
        config["app"]["upload_path"] = "json_upload_path";

        app().loadConfigJson(config);

        CHECK(app().getThreadNum() == 10);
        CHECK(app().getUploadPath().find("json_upload_path") != std::string::npos);
    }

    // Test loadConfigFile
    {
        std::string filename = "test_config.json";
        std::ofstream outfile(filename);
        Json::Value config;
        config["app"]["threads_num"] = 16;
        config["app"]["upload_path"] = "file_upload_path";
        outfile << config;
        outfile.close();

        app().loadConfigFile(filename);

        CHECK(app().getThreadNum() == 16);
        CHECK(app().getUploadPath().find("file_upload_path") != std::string::npos);

        std::filesystem::remove(filename);
    }
}

int main(int argc, char **argv)
{
    return drogon::test::run(argc, argv);
}

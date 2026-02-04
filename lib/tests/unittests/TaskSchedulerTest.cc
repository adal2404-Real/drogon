#include <drogon/drogon_test.h>
#include <drogon/TaskScheduler.h>
#include <drogon/HttpAppFramework.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace drogon;

DROGON_TEST(TaskSchedulerTest)
{
    auto &scheduler = TaskScheduler::instance();

    SUBSECTION(RunAfter)
    {
        std::shared_ptr<std::atomic<bool>> called = std::make_shared<std::atomic<bool>>(false);
        auto id = scheduler.runAfter(0.01, [called](){
            *called = true;
        });
        CHECK(id > 0);

        // Wait for it
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CHECK(*called == true);
    }

    SUBSECTION(RunEvery)
    {
        std::shared_ptr<std::atomic<int>> calls = std::make_shared<std::atomic<int>>(0);
        auto id = scheduler.runEvery(0.02, [calls](){
            (*calls)++;
        });
        CHECK(id > 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        scheduler.invalidate(id);
        int c1 = *calls;
        CHECK(c1 >= 3); // 0.02s * 3 = 0.06s < 0.1s

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        CHECK(*calls == c1); // Should stop
    }

    SUBSECTION(RunInThreadPool)
    {
        std::shared_ptr<std::atomic<bool>> called =
            std::make_shared<std::atomic<bool>>(false);
        auto tid = std::this_thread::get_id();
        scheduler.runTaskInThreadPool([called, tid]() {
            if (std::this_thread::get_id() != tid)
                *called = true;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CHECK(*called == true);
    }
}

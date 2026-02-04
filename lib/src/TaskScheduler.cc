/**
 *
 *  @file TaskScheduler.cc
 *
 *  Copyright 2024, Drogon framework.  All rights reserved.
 *  https://github.com/drogonframework/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#include <drogon/TaskScheduler.h>
#include <drogon/HttpAppFramework.h>
#include <thread>

namespace drogon
{

TaskScheduler &TaskScheduler::instance()
{
    static TaskScheduler inst;
    return inst;
}

TaskScheduler::TaskScheduler()
{
}

TaskScheduler::~TaskScheduler()
{
}

trantor::TimerId TaskScheduler::runAfter(double delay, const std::function<void()> &task)
{
    return app().getLoop()->runAfter(delay, task);
}

trantor::TimerId TaskScheduler::runEvery(double interval, const std::function<void()> &task)
{
    return app().getLoop()->runEvery(interval, task);
}

trantor::TimerId TaskScheduler::runAt(const trantor::Date &date, const std::function<void()> &task)
{
    return app().getLoop()->runAt(date, task);
}

void TaskScheduler::invalidate(trantor::TimerId id)
{
    app().getLoop()->invalidateTimer(id);
}

void TaskScheduler::runTaskInThreadPool(const std::function<void()> &task)
{
    std::thread([task]() { task(); }).detach();
}

}

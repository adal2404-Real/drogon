/**
 *
 *  @file TaskScheduler.h
 *
 *  Copyright 2024, Drogon framework.  All rights reserved.
 *  https://github.com/drogonframework/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#pragma once

#include <drogon/exports.h>
#include <trantor/utils/Date.h>
#include <trantor/net/EventLoop.h>
#include <trantor/utils/NonCopyable.h>
#include <functional>
#include <memory>
#include <string>

namespace drogon
{

/**
 * @brief A task scheduler for running tasks at specific times or intervals.
 * Supports fire-and-forget tasks and named persistent tasks.
 */
class DROGON_EXPORT TaskScheduler : public trantor::NonCopyable
{
public:
    static TaskScheduler &instance();

    /**
     * @brief Run a task after a delay.
     * @param delay The delay in seconds.
     * @param task The task to run.
     * @return trantor::TimerId The timer ID.
     */
    trantor::TimerId runAfter(double delay, const std::function<void()> &task);

    /**
     * @brief Run a task every interval.
     * @param interval The interval in seconds.
     * @param task The task to run.
     * @return trantor::TimerId The timer ID.
     */
    trantor::TimerId runEvery(double interval, const std::function<void()> &task);

    /**
     * @brief Run a task at a specific time.
     * @param date The time to run the task.
     * @param task The task to run.
     * @return trantor::TimerId The timer ID.
     */
    trantor::TimerId runAt(const trantor::Date &date, const std::function<void()> &task);

    /**
     * @brief Invalidate a timer.
     * @param id The timer ID.
     */
    void invalidate(trantor::TimerId id);

    /**
     * @brief Run a task in a thread pool (detached thread).
     * @param task The task to run.
     */
    void runTaskInThreadPool(const std::function<void()> &task);

private:
    TaskScheduler();
    ~TaskScheduler();
    trantor::EventLoop *loop_{nullptr};
};

}

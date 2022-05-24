#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <future>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include <atomic>
#include <variant>
#include <cassert>
#include <map>
#include <utility>
#include <functional>

enum class TaskStatus {
    in_q,
    completed
};

class Task 
{
public:
    template <typename FuncRetType, typename ...Args, typename ...FuncTypes>
    Task(FuncRetType(*func)(FuncTypes...), Args&&... args) :
        is_void{ std::is_void_v<FuncRetType> } {

        void_func = std::bind(func, args...);

        any_func = []()->int { return 0; };
    }

    void operator() ();

    bool has_result();

    std::any get_result() const;

private:
    std::function<void()> void_func;

    std::function<std::any()> any_func;

    std::any any_func_result;

    bool is_void;
};

struct TaskInfo 
{
    TaskStatus status = TaskStatus::in_q;

    std::any result;
};


class thread_pool 
{
public:
    thread_pool(const uint32_t num_threads) 
    {
        threads.reserve(num_threads);
        for (int i = 0; i < num_threads; ++i) 
        {
            threads.emplace_back(&thread_pool::run, this);
        }
    }

    template <typename Func, typename ...Args, typename ...FuncTypes>
    uint64_t add_task(Func(*func)(FuncTypes...), Args&&... args) 
    {

        const uint64_t task_id = last_idx++;

        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info[task_id] = TaskInfo();
        lock.unlock();

        std::lock_guard<std::mutex> q_lock(q_mtx);
        q.emplace(Task(func, std::forward<Args>(args)...), task_id);
        q_cv.notify_one();

        return task_id;
    }

    void wait(const uint64_t task_id);

    void wait_all();

    ~thread_pool() 
    {
        quite = true;
        q_cv.notify_all();
        for (int i = 0; i < threads.size(); ++i) 
        {
            threads[i].join();
        }
    }

private:

    void run() 
    {
        while (!quite) 
        {
            std::unique_lock<std::mutex> lock(q_mtx);
            q_cv.wait(lock, [this]()->bool { return !q.empty() || quite; });

            if (!q.empty() && !quite) 
            {
                std::pair<Task, uint64_t> task = std::move(q.front());
                q.pop();
                lock.unlock();

                task.first();

                std::lock_guard<std::mutex> lock(tasks_info_mtx);
                if (task.first.has_result()) {
                    tasks_info[task.second].result = task.first.get_result();
                }
                tasks_info[task.second].status = TaskStatus::completed;
                ++cnt_completed_tasks;
            }
            wait_all_cv.notify_all();
            tasks_info_cv.notify_all(); // notify for wait function
        }
    }

    std::vector<std::thread> threads;

    std::queue<std::pair<Task, uint64_t>> q;
    std::mutex q_mtx;
    std::condition_variable q_cv;

    std::unordered_map<uint64_t, TaskInfo> tasks_info;
    std::condition_variable tasks_info_cv;
    std::mutex tasks_info_mtx;

    std::condition_variable wait_all_cv;

    std::atomic<bool> quite{ false };
    std::atomic<uint64_t> last_idx{ 0 };
    std::atomic<uint64_t> cnt_completed_tasks{ 0 };
};

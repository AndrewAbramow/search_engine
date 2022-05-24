
#include "ThreadPool.h"

    void Task::operator() () 
    {
        void_func();
        any_func_result = any_func();
    }

    bool Task::has_result() 
    {

        return !is_void;
    }

    std::any Task::get_result() const 
    {
        assert(!is_void);
        assert(any_func_result.has_value());

        return any_func_result;
    }

    void thread_pool::wait(const uint64_t task_id) 
    {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool 
        {
            return task_id < last_idx&& tasks_info[task_id].status == TaskStatus::completed;
        });
    }

    void thread_pool::wait_all() 
    {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        wait_all_cv.wait(lock, [this]()->bool 
        {
                return cnt_completed_tasks == last_idx; 
        });
    }
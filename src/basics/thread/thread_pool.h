#ifndef _W_THREAD_POOL_H_
#define _W_THREAD_POOL_H_

#include "wthread.h"
#include <queue>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

namespace gdres {

class ThreadPool
{
public:
    ThreadPool(const ThreadPool&) = delete;   // 禁用拷贝构造
    ThreadPool(ThreadPool&&) = delete;   // 禁用移动构造
    ThreadPool& operator=(const ThreadPool&) = delete; // 禁用赋值操作
    ThreadPool& operator=(ThreadPool&&) = delete; // 禁用移动赋值操作

    ThreadPool(int min, int max, const std::string& nm = ""); // 动态线程，管理函数进行调度
    ThreadPool(int threadNum, const std::string& nm = "");    // 固定线程，不需要管理函数
    ~ThreadPool();

    void AddTask(std::function<void()> cb);               // 给线程池添加任务
    void AddThread();                                     // 向线程池中增加工作线程

    int GetBusyNum() const {return m_busyNum;} 
    int GetLiveNum() const {return m_liveNum;}
    int GetTaskNum() const {return m_tasks.size();}

    // 给线程池添加任务
    // template <typename F, typename... Args>
    // auto AddTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;

private:
    void Init();                   // 初始化线程池
    void Worker();                 // 工作线程      
    void Manager();                // 管理线程
    void Destroy();                // 销毁线程池

private:
    std::queue<std::function<void()>> m_tasks;      // 任务队列
    std::string m_name;                             // 线程池名字

    Thread* m_managerTh;                           // 管理线程
    std::vector<Thread*> m_workThs;                // 工作线程组
    int m_minNum;                                  // 最少线程数
    int m_maxNum;                                  // 最多线程数
    std::atomic<uint16_t> m_busyNum;               // 忙碌的线程数
    std::atomic<uint16_t> m_liveNum;               // 存活线程数,等于工作线程数组的大小
    std::atomic<uint16_t> m_exitNum;               // 要销毁的线程数
    static const int NUMBER = 2;                   // 每次添加或删除两个线程

    std::mutex m_mutexPool;                        // 锁整个线程池
    Semaphore m_sem;                               // 信号量

    bool m_shutdown;                               // 销毁线程池
};

}

#endif

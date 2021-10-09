/* =======================================================================
 * @brief  : 协程调度模块（支持协程在线程池里边切换）
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */
#ifndef _GDRES_SCHEDULER_H_
#define _GDRES_SCHEDULER_H_

#include <memory>
#include <mutex>
#include "fiber.h"
#include <vector>
#include "wthread.h"
#include <list>
#include <condition_variable>

namespace gdres {

#define THREAD_CNT_DEFAULT 4

class Scheduler
{
public:
    typedef std::mutex MutexType;  // 此种写法方便对锁进行替换

    // use_caller 是否将当前线程纳入到调度器中进行管理
    // 暂不考虑将当前线程纳入管理，创建调度的线程为主线程，只负责启停调度器，不占用cpu资源，只是多出一条空闲线程，不影响性能，以后优化
    Scheduler(size_t threads = THREAD_CNT_DEFAULT, const std::string& name = "", bool useCaller = false);
    virtual ~Scheduler();

    void Start();                                              // 启动调度器
    void Stop();                                               // 停止调度器
    const std::string& GetName() const { return m_name;}       // 获取调度器名称

    static Scheduler* GetThis();                               // 获取调度器指针
    static Fiber* GetMainFiber();                            

    // 添加任务
    void AddTask(Fiber* fib, int64_t thId = -1);
    void AddTask(std::function<void()> cb, int64_t thId = -1);


protected:
    void Run();                                   // 调度器执行函数
    void SetThis();                               // 设置当前线程的协程调度器

    virtual void Tickle();                        // 通知协程调度器有任务了
    virtual void Idle();                          // 协程无任务可调度时执行idle协程

private:
    struct FiberAndThread 
    {
        int64_t thId;                     // 线程id
        Fiber* fiber;                 // 协程
        std::function<void()> func;   // 执行的函数
        // 执行某个线程上的某个协程
        FiberAndThread(int64_t id, Fiber* fib) : thId(id), fiber(fib), func(nullptr) {}
        // 某个线程上执行任务
        FiberAndThread(int64_t id, std::function<void()> fun) : thId(id), func(fun), fiber(nullptr) {}
        // 默认构造
        FiberAndThread() : thId(-1), fiber(nullptr), func(nullptr) {}

        // 重置方法
        void Reset()
        {
            thId = -1;
            fiber = nullptr;
            func = nullptr;
        }
    };

private:
    MutexType m_mutex; 
    std::condition_variable isIdle;
                         
    std::vector<Thread*> m_threads;         // 线程池
    std::list<FiberAndThread> m_tasks;      // 待执行协程任务队列

    std::string m_name;                     // 协程调度器名称
    Fiber* m_rootFib;                       // use_caller为true时有效，即创建调度器的那个线程转化的协程

protected:
    std::vector<int64_t> m_threadIds;              // 线程id数组
    size_t m_threadCnt;                            // 需要创建的线程数量
    std::atomic<size_t> m_activeThreadCnt;         // 活跃线程数量
    std::atomic<size_t> m_idleThreadCnt;           // 空闲线程数量
    bool m_stopping;                               // 调度器退出控制
    bool m_autoStop;
    int64_t m_rootThreadId;                        // 主线程id
};




















}






#endif


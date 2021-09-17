#ifndef W_THREAD_POOL_H
#define W_THREAD_POOL_H

#include <thread>
#include <queue>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <safe_queue.h>
#include <functional>
#include <future>

class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(int min, int max); // 动态线程，管理函数进行调度
    ThreadPool(int threadNum);  // 固定线程，不需要管理函数
    ~ThreadPool();

    // 初始化线程池
    void Init();

    // 给线程池添加任务
    template <typename F, typename... Args>
    auto AddTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;

    // 获取线程池中工作的线程的个数
    int GetBusyNum();
    // 获取线程池中活着的线程的个数
    int GetLiveNum();

    // 添加工作线程
    void AddThread();

    int GetTaskNum() {return taskQ.size();}

private:
    // Worker函数 读取任务队列进行任务处理
    void Worker();
    // 管理者线程
    void Manager();
    // 线程退出
    void ThreadExit();
    // 销毁线程池
    void Destroy();

    class WorkThread
    {
    public:
        WorkThread() : isExit(false), isEnd(false), pool(nullptr) {}
        WorkThread(ThreadPool* thPool) : isExit(false), isEnd(false), pool(thPool) {Start();}
        ~WorkThread() {Stop();}

        // void operator()();
        bool GetStatus() {return isEnd;}
    private:
        void ThMain();
        void Start();
        void Stop();

        std::thread th;
        bool isExit;   // 控制线程退出标志，在ThMain函数运行条件中进行判断
        bool isEnd;    // 线程执行状态标志，在ThMain函数退出前置为true
        ThreadPool* pool;
    };


private:
    SafeQueue<std::function<void()>> taskQ;      // 任务队列

    std::thread managerTh;      // 管理者线程
    // 注意：由于工作线程类在构造和析构函数中均涉及线程操作，所以要防止隐式的拷贝构造
    // 所以此处的工作线程组需要用类指针进行管理
    std::vector<WorkThread*> workThs;  // 工作线程组
    int minNum;            // 最少线程数
    int maxNum;            // 最多线程数
    int busyNum;           // 忙碌的线程数
    int liveNum;           // 存活线程数,等于工作线程数组的大小
    int exitNum;           // 要销毁的线程数
    static const int NUMBER = 2; // 每次添加或删除两个线程

    std::mutex mutexPool;  // 锁整个线程池
    std::condition_variable isEmpty;    // 是否空了

    bool shutdown;          // 是不是要销毁线程池，1销毁，0不销毁
};

#endif

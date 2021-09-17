/*==================================================================
 * description : 线程池的实现
 * explain : c++ 11 thread 实现跨平台线程池
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/

#include <iostream>
#include <string.h> // c中的字符串操作函数，拷贝等函数
#include <cstdio>   // printf用
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <unistd.h>
#include <safe_queue.h>
#include <thread_pool.h>
#include <functional>
#include <future>
#include <wtimer.h>

/*==================================================================
 * FuncName : ThreadPool::ThreadPool
 * description : 
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
ThreadPool::ThreadPool(int min, int max) : minNum(min), maxNum(max),
                                           busyNum(0), liveNum(0), exitNum(0), shutdown(false)
{
    workThs.clear();
}

ThreadPool::ThreadPool(int threadNum) : minNum(threadNum), maxNum(threadNum),
                                        busyNum(0), liveNum(0), exitNum(0), shutdown(false)
{
    workThs.clear();
}

ThreadPool::ThreadPool() : minNum(3), maxNum(8), busyNum(0), liveNum(0),
                           exitNum(0), shutdown(false)
{
    workThs.clear();
}


ThreadPool::~ThreadPool()
{
    Destroy();
}

/*==================================================================
 * FuncName : ThreadPool::Init
 * description : 初始化线程池
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
void ThreadPool::Init()
{
    // 创建管理者线程,如果是固定线程数量则不需要管理者线程
    if (minNum != maxNum) {
        managerTh = std::thread(&ThreadPool::Manager, this);
    }

    // 创建工作线程
    for (int i = 0; i < minNum; i++) {
        liveNum++;
        workThs.push_back(new WorkThread(this));
    }
}

/*==================================================================
 * FuncName : ThreadPool::Manager
 * description : 管理者线程 每隔3s进行一次线程调度
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
void ThreadPool::Manager()
{
    while (!shutdown) {
        // 每隔3s检测一次
        wsleeps(3);
        int queSize = taskQ.size();
        std::unique_lock<std::mutex> lck(mutexPool);         
        int aliveNum = liveNum;
        int busyNumber = busyNum;
        lck.unlock();

        // std::cout << "queSize is : " << queSize << std::endl;
        // std::cout << "aliveNum is : " << aliveNum << std::endl;
        // std::cout << "busyNumber is : " << busyNumber << std::endl;

        // 添加线程,简单逻辑判断
        if ((queSize > aliveNum) && (aliveNum < maxNum)) {
            // 每次添加固定数量的线程
            lck.lock();
            for (int i = 0; (i < NUMBER) && (liveNum < maxNum); i++) {
                liveNum++;
                workThs.push_back(new WorkThread(this)); 
            }
            lck.unlock();
        }
        

        // 任务量小的时候杀掉多余的线程，每次杀掉固定数量线程
        if (busyNumber * 2 < aliveNum && aliveNum > minNum) {
            lck.lock();
            exitNum = NUMBER;
            lck.unlock();
            // 唤醒工作线程进行自杀
            for (int i = 0; i < NUMBER; i++) {
                isEmpty.notify_all();
            }
            
            wsleeps(1);
            // 清理已经退出的工作线程
            lck.lock();
            for (auto it = workThs.begin(); it != workThs.end(); it++) {
                if ((*it)->GetStatus()) {
                    liveNum--;
                    delete *it;
                    it = workThs.erase(it);
                }
                if (it == workThs.end()) {
                    break;
                }
            }
            lck.unlock();
        }
    }
    std::cout << "manager thread exit" << std::endl;
    return;
}

/*==================================================================
 * FuncName : ThreadPool::AddTask
 * description : 给线程池添加任务
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
template <typename F, typename... Args>
auto ThreadPool::AddTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
{
    
    // 创建一个具有已准备好执行的有界参数的函数
    std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    // 将其封装到共享指针中，以便能够复制构造
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

    // 将task_ptr指向的std::packaged_task对象取出，并包装为void函数
    std::function<void()> warpper_func = [task_ptr]()
    {
        (*task_ptr)();
    };

    // 将任务添加进队列    
    taskQ.push(warpper_func);

    // 唤醒消费者线程
    isEmpty.notify_all();

    return task_ptr->get_future();
}

/*==================================================================
 * FuncName : ThreadPool::AddThread
 * description : 添加工作线程
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
void ThreadPool::AddThread()
{
    std::unique_lock<std::mutex> lck(mutexPool);
    liveNum++;
    workThs.push_back(new WorkThread(this));
}

/*==================================================================
 * FuncName : ThreadPool::GetBusyNum
 * description : 获取线程池中工作的线程的个数
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
int ThreadPool::GetBusyNum()
{
    std::unique_lock<std::mutex> lck(mutexPool);
    return busyNum;
}

/*==================================================================
 * FuncName : ThreadPool::GetLiveNum
 * description : 获取线程池中活着的线程的个数
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
int ThreadPool::GetLiveNum()
{
    std::unique_lock<std::mutex> lck(mutexPool);
    return liveNum;
}

/*==================================================================
 * FuncName : ThreadPool::Destroy
 * description : 销毁线程池
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
void ThreadPool::Destroy()
{
    // 关闭线程池
    shutdown = true;

    // 阻塞回收管理者线程
    if (managerTh.joinable()) {
        managerTh.join();
    }

    // 唤醒阻塞的工作线程并销毁
    int num = GetLiveNum();
    for (int i = 0; i < num; i++) {
        isEmpty.notify_all();
    }

    wsleep(500); // 等待阻塞的子线程退出

    for (int n = 0; n < workThs.size(); n++) {
        // 销毁工作线程对象时会调用析构函数，stop方法保证子线程退出 忙碌中的子线程会通过join等待退出
        delete workThs[n];
    }
  
    workThs.clear();

    return;
}


/*==================================================================
 * FuncName : ThreadPool::WorkThread::ThMain
 * description : Worker线程 读取任务队列进行任务处理
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
void ThreadPool::WorkThread::ThMain()
{
    std::function<void()> func;
    while (!isExit) {
        // 加锁访问线程池
        std::unique_lock<std::mutex> lck(pool->mutexPool);
        // 当前任务数量是否为空或者线程池是否要被销毁
        while (pool->taskQ.empty() && !pool->shutdown) {
            
            // 阻塞线程，等待被唤醒
            // std::cout << "WorkThread wait" << std::endl;
            pool->isEmpty.wait(lck);

            // 判断有没有线程需要销毁
            if (pool->exitNum > 0) {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum) {
                    isEnd = true; // 将线程退出状态置位true
                    std::cout << "work thread exit : " << std::this_thread::get_id() << std::endl;    
                    return; // 线程结束自动销毁                 
                }
            }
        }

        // 判断当前线程池是否被关闭
        if (pool->shutdown) {
            isEnd = true;
            // std::cout << "work thread exit : " << std::this_thread::get_id() << std::endl; 
            return;
        }

        // 从任务队列中取出一个任务,先将忙碌线程数加一，调用完成后再减一
        if (pool->taskQ.take(func)) {
            pool->busyNum++;
            lck.unlock();
            // std::cout << "work thread start : " << std::this_thread::get_id() << std::endl; 

            func();  // 执行任务函数

            // std::cout << "work thread end : " << std::this_thread::get_id() << std::endl; 
            lck.lock();
            pool->busyNum--;
            lck.unlock();
        }
    }
    isEnd = true;
    std::cout << "work thread exit : " << std::this_thread::get_id() << std::endl; 
    return;
}

/*==================================================================
 * FuncName : ThreadPool::WorkThread::Start
 * description : Worker线程启动
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
void ThreadPool::WorkThread::Start()
{
    th = std::thread(&ThreadPool::WorkThread::ThMain, this);
}

/*==================================================================
 * FuncName : ThreadPool::WorkThread::Stop
 * description : Worker线程停止
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
void ThreadPool::WorkThread::Stop()
{
    isExit = true;
    if (th.joinable()) {
        th.join();
    }
}

/*==================================================================
 * FuncName : ThreadPool::WorkThread::Stop
 * description : Worker线程停止
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
// void ThreadPool::WorkThread::operator()()
// {
//     // Start();
// }
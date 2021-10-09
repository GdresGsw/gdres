/*==================================================================
 * description : 线程池的实现
 * explain : c++ 11 thread 实现跨平台线程池
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/

#include <iostream>
#include <string.h> // c中的字符串操作函数，拷贝等函数
#include <cstdio>   // printf用
#include "thread_pool.h"
#include "timer.h"
#include "log.h"

namespace gdres {




/*==================================================================
 * FuncName : ThreadPool::ThreadPool
 * description : 
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
ThreadPool::ThreadPool(int min, int max, const std::string& nm) : m_managerTh(nullptr), m_name(nm),
            m_minNum(min), m_maxNum(max), m_busyNum(0), m_liveNum(0), m_exitNum(0), m_shutdown(false)
{
    m_workThs.reserve(min);
    Init();
}

ThreadPool::ThreadPool(int threadNum, const std::string& nm) : m_managerTh(nullptr), m_name(nm), m_minNum(threadNum),
                       m_maxNum(threadNum), m_busyNum(0), m_liveNum(0), m_exitNum(0), m_shutdown(false)
{
    m_workThs.reserve(threadNum);
    Init();
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
    if (m_minNum != m_maxNum) {
        m_managerTh = new Thread(std::bind(&ThreadPool::Manager, this), m_name + "_" + "mgr");
    }

    // 创建工作线程
    for (int i = 0; i < m_minNum; i++) {
        m_liveNum++;
        m_workThs[i] = new Thread(std::bind(&ThreadPool::Worker, this),
                                  m_name + "_" + "wkr_" + std::to_string(m_liveNum));
    }

    GDRES_LOG(LOG_EVENT, "threadPool(%s) init OK!", m_name.c_str());
}

/*==================================================================
 * FuncName : ThreadPool::Manager
 * description : 管理者线程
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
void ThreadPool::Manager()
{
    int taskCnt = 0;      // 任务数量
    int aliveCnt = 0;     // 存活线程数量
    int busyCnt = 0;      // 忙碌线程数量

    while (!m_shutdown) {
        SLEEPS(3);  // 每隔3s检测一次

        std::unique_lock<std::mutex> lck(m_mutexPool);    
        taskCnt = m_tasks.size();     
        aliveCnt = m_liveNum;
        busyCnt = m_busyNum;

        // 添加线程,简单逻辑判断
        if ((taskCnt > aliveCnt) && (aliveCnt < m_maxNum)) {
            // 每次添加固定数量的线程
            for (int i = 0; (i < NUMBER) && (m_liveNum < m_maxNum); i++) {
                m_liveNum++;
                m_workThs.push_back(new Thread(std::bind(&ThreadPool::Worker, this),
                                               m_name + "_" + "wkr_" + std::to_string(m_liveNum))); 
            }
        }

        // 清理已经退出的工作线程
        for (auto it = m_workThs.begin(); it != m_workThs.end(); it++) {
            if ((*it)->getstate() == Thread::TERM) {
                m_liveNum--;
                delete (*it);
                it = m_workThs.erase(it);
            }
            if (it == m_workThs.end()) {
                break;
            }
        }

        // 任务量小的时候杀掉多余的线程，每次杀掉固定数量线程
        if (busyCnt * 2 < aliveCnt && aliveCnt > m_minNum) {
            m_exitNum = NUMBER;
            // 唤醒工作线程进行自杀
            for (int i = 0; i < NUMBER; i++) {
                m_sem.signal();
            }
        }
    }

    GDRES_LOG(LOG_EVENT, "thread(%s) over!", m_name + "_" + "mgr");
    return;
}

/*==================================================================
 * FuncName : ThreadPool::Worker
 * description : Worker线程 读取任务队列进行任务处理
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/ 
void ThreadPool::Worker()
{
    // 此处线程不工作时阻塞，或者让线程执行空闲函数，以后根据需要再修改
    std::function<void()> func;
    while (!m_shutdown) {
        m_sem.wait();
        {   
            std::unique_lock<std::mutex> lck(m_mutexPool);
            // 判断当前线程池是否被关闭
            if (m_shutdown) {
                LOG_SS() << "work thread exit";
                return;
            }

            // 判断有没有线程需要销毁
            if (m_exitNum > 0) {
                m_exitNum--;
                if (m_liveNum > m_minNum) {
                    LOG_SS() << "work thread exit";    
                    return; // 线程结束自动销毁                 
                }
            }

            // 确认确实有任务可取
            if (m_tasks.empty()) {
                continue;
            }

            func = m_tasks.front();
            m_tasks.pop();
        }

        m_busyNum++;
        if (func) {
            func();
        }
        m_busyNum--;
    }

    LOG_SS() << "work thread exit";
    return;
}

/*==================================================================
 * FuncName : ThreadPool::AddTask
 * description : 给线程池添加任务
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
void ThreadPool::AddTask(std::function<void()> cb)
{
    m_mutexPool.lock();
    m_tasks.push(cb);    
    m_mutexPool.unlock();

    m_sem.signal();
}

/*==================================================================
 * FuncName : ThreadPool::AddThread
 * description : 添加工作线程
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
void ThreadPool::AddThread()
{
    std::unique_lock<std::mutex> lck(m_mutexPool);
    m_liveNum++;
    m_workThs.push_back(new Thread(std::bind(&ThreadPool::Worker, this),
                                             m_name + "_" + "wkr_" + std::to_string(m_liveNum)));
}

/*==================================================================
 * FuncName : ThreadPool::Destroy
 * description : 销毁线程池
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
void ThreadPool::Destroy()
{
    // 关闭线程池
    m_shutdown = true;

    // 回收管理者线程
    if (m_managerTh) {
        m_managerTh->join();
    }
    
    // 唤醒阻塞的工作线程并销毁
    for (int i = 0; i < m_liveNum; i++) {
        m_sem.signal();
    }

    for (int n = 0; n < m_workThs.size(); n++) {
        // 忙碌中的子线程会通过join等待退出
        m_workThs[n]->join();
    }

    bool ret = m_managerTh->getstate() == Thread::TERM;
    for (auto it : m_workThs) {
        ret &= (it->getstate() == Thread::TERM);
    }

    if (!ret) {
        GDRES_LOG(LOG_EVENT, "threadPool(%s) exit error!", m_name);
    }

    delete m_managerTh;
    for (int n = 0; n < m_workThs.size(); n++) {
        delete m_workThs[n];
    }
    
    m_workThs.clear();

    return;
}










/*==================================================================
 * FuncName : ThreadPool::AddTask
 * description : 给线程池添加任务
 * author : guoshuaiwei  2021/9/1
 *==================================================================*/
// template <typename F, typename... Args>
// auto ThreadPool::AddTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
// {
    
//     // 创建一个具有已准备好执行的有界参数的函数
//     std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

//     // 将其封装到共享指针中，以便能够复制构造
//     auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

//     // 将task_ptr指向的std::packaged_task对象取出，并包装为void函数
//     std::function<void()> warpper_func = [task_ptr]()
//     {
//         (*task_ptr)();
//     };

//     // 将任务添加进队列    
//     taskQ.push(warpper_func);

//     // 唤醒消费者线程
//     isEmpty.notify_all();

//     return task_ptr->get_future();
// }

}


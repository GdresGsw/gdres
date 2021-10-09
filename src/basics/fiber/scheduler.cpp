/* =======================================================================
 * @brief  : 协程调度模块（支持协程在线程池里边切换）
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */
#include "scheduler.h"
#include "func_pub.h"
#include "log.h"
#include "timer.h"

namespace gdres {

static thread_local Scheduler* g_scheduler = nullptr;
static thread_local Fiber* g_schedulerFiber = nullptr;

Scheduler* Scheduler::GetThis() {return g_scheduler;}
Fiber* Scheduler::GetMainFiber() {return g_schedulerFiber;}

/*==================================================================
 * FuncName : Scheduler::Scheduler
 * description : 构造
 * author : guoshuaiwei
 *==================================================================*/
Scheduler::Scheduler(size_t thCnt, const std::string& name, bool useCaller) : m_name(name), m_rootFib(nullptr), m_activeThreadCnt(0),
                                                                              m_idleThreadCnt(0), m_stopping(false), m_autoStop(false)
{
    size_t threadCnt = thCnt < THREAD_CNT_DEFAULT ? THREAD_CNT_DEFAULT : thCnt;  // 默认至少有4个线程
    
    // 如果将主线程纳入调度器管理（创建调度器的线程为主线程）
    if (useCaller) {
        Thread* th = gdres::Thread::GetThis();
        gdres::Thread::SetThreadName(m_name);  // 将主线程命名为调度器的名字        
        gdres::Fiber::GetThis();               // 将当前线程转换成协程
        threadCnt--;

        g_scheduler = this;
        m_rootFib = new Fiber(std::bind(&Scheduler::Run, this), 0);
        g_schedulerFiber = m_rootFib;

        m_rootThreadId = th->getid();
        m_threadIds.clear();
        m_threadIds.emplace_back(m_rootThreadId); 
    } else {
        m_rootThreadId = -1;
    }

    m_threadCnt = threadCnt;
}

Scheduler::~Scheduler()
{
    if(GetThis() == this) {
        LOG_SS() << g_scheduler;
        g_scheduler = nullptr;
    }
}

void Scheduler::SetThis()
{
    g_scheduler = this;
}

/*==================================================================
 * FuncName : Scheduler::Start
 * description : 启动调度器
 * author : guoshuaiwei
 *==================================================================*/
void Scheduler::Start()
{
    m_threads.resize(m_threadCnt);
    for (int i = 0; i < m_threadCnt; i++) {
        m_threads[i] = new gdres::Thread(std::bind(&Scheduler::Run, this),
                                         m_name + "_" + std::to_string(i));
        m_threadIds.push_back(m_threads[i]->getid());
    }

    if (m_rootFib) {
        m_rootFib->SwapIn();
    }

    while (m_idleThreadCnt.load() != m_threadCnt) {
        SLEEPMS(500);
        LOG_SS() << "Scheduler start thread count : " << m_idleThreadCnt.load();     
    }
    LOG_SS() << "Scheduler start ok! thread count : " << m_idleThreadCnt.load();
}
/*==================================================================
 * FuncName : Scheduler::AddTask
 * description : 添加任务
 * author : guoshuaiwei
 *==================================================================*/
void Scheduler::AddTask(Fiber* fib, int64_t thId)
{
    if (!fib) {
        return;
    }
    FiberAndThread ft(thId, fib);
    std::unique_lock<std::mutex> lck(m_mutex);
    m_tasks.push_back(ft);
    Tickle();
}

void Scheduler::AddTask(std::function<void()> cb, int64_t thId)
{
    FiberAndThread ft(thId, cb);
    std::unique_lock<std::mutex> lck(m_mutex);
    m_tasks.push_back(ft);
    Tickle();
}

/*==================================================================
 * FuncName : Scheduler::Run
 * description : 调度器执行函数
 * author : guoshuaiwei
 *==================================================================*/
void Scheduler::Run()
{
    SetThis();
    if (gdres::GetThreadId() != m_rootThreadId) {
        g_schedulerFiber = gdres::Fiber::GetThis();
    }

    Fiber* idleFib = new Fiber(std::bind(&Scheduler::Idle, this));
    Fiber* cbFiber = new Fiber(nullptr);
    FiberAndThread ft = {};

    while (!m_stopping) {
        ft.Reset();
        bool tickle = false;    // 是否需要通知其他线程做任务
        bool isActive = false;  // 当前线程是否活跃,区分没任务还是拿到了无效任务
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            auto task = m_tasks.begin();
            while (task != m_tasks.end()) {
                // 只做要在当前线程进行的任务和没有指定线程id的任务
                if (task->thId != -1 && task->thId != gdres::GetThreadId()) {
                    task++;
                    tickle = true;
                    continue;
                }

                if (task->fiber && task->fiber->GetState() == Fiber::EXEC) {
                    task++;
                    continue;
                }

                ft = *task;
                m_tasks.erase(task++);
                m_activeThreadCnt++;
                isActive = true;
                break;
            }
            // 如果需要其他线程处理，或者还有任务的时候，通知其他线程处理
            tickle = tickle || task != m_tasks.end();
        }

        if (tickle) {
            Tickle();  // 唤醒其他线程
        }

        if (ft.fiber && (ft.fiber->GetState() != Fiber::TERM &&
                         ft.fiber->GetState() != Fiber::EXCEPT)) {
            // 如果任务中的fiber可被执行
            ft.fiber->SwapIn();
            m_activeThreadCnt--;

            // 任务执行后的几种状态：
            // 1. READY 状态，则需要继续将该任务放入任务队列
            // 2. 正常执行完或者异常，则将协程置为hold状态
            if (ft.fiber->GetState() == Fiber::READY) {
                AddTask(ft.fiber);
            } else if (ft.fiber->GetState() != Fiber::TERM &&
                       ft.fiber->GetState() != Fiber::EXCEPT) {
                ft.fiber->fibState = Fiber::HOLD;
            }
        } else if (ft.func) {
            // 如果任务中的func可被执行
            cbFiber->Reset(ft.func);
            cbFiber->SwapIn();
            m_activeThreadCnt--;

            // 对于func的执行，只有当状态为READY的时候才处理，其他状态则不再处理
            if (cbFiber->GetState() == Fiber::READY) {
                AddTask(cbFiber);
            };
            cbFiber->Reset(nullptr);
        } else {
            // 如果拿到了无效任务
            if (isActive) {
                m_activeThreadCnt--;
                continue;
            }

            // 如果没任务，则执行idleFib
            if (idleFib->GetState() == Fiber::TERM) {
                LOG_SS() << "idle fiber term";
                break;
            }

            m_idleThreadCnt++;
            idleFib->SwapIn();
            m_idleThreadCnt--;

            if (idleFib->GetState() != Fiber::TERM && 
                idleFib->GetState() != Fiber::EXCEPT) {
                idleFib->fibState = Fiber::HOLD;
            }    
        }
    }

    LOG_SS() << "thread term";

    delete idleFib;
    delete cbFiber;
    delete g_schedulerFiber;
    g_schedulerFiber = nullptr;
}

/*==================================================================
 * FuncName : Scheduler::Stop
 * description : 调度器停止函数
 * author : guoshuaiwei
 *==================================================================*/
void Scheduler::Stop()
{
    // 调度器的退出有两种情况：主动退出，或者各个线程结束了，自动退出
    // 主线程调用该停止方法的时候，各个子线程会有三种情况：
    // 在执行任务Fiber，在执行idleFiber，线程已自己结束
    m_stopping = true;  // 结束在执行任务Fiber
    int cnt = m_idleThreadCnt;
    for (int i = 0; i < cnt; i++) {
        Tickle();
    }

    for (auto &it : m_threads) {
        delete it;
    }

    m_threads.clear();
}

/*==================================================================
 * FuncName : Scheduler::Idle
 * description : 空闲协程执行
 * author : guoshuaiwei
 *==================================================================*/
void Scheduler::Idle()
{
    while (!m_stopping) {
        // 进入此处就说明无任务可做
        // LOG_SS() << "Idle in";
        {
        std::unique_lock<std::mutex> lck(m_mutex);
        isIdle.wait(lck);
        }
        gdres::Fiber::YieldToHold();
    }
}

/*==================================================================
 * FuncName : Scheduler::Tickle
 * description : 唤醒其他其他线程
 * author : guoshuaiwei
 *==================================================================*/
void Scheduler::Tickle()
{
    isIdle.notify_one();
}



















}


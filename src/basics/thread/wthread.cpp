/* =======================================================================
 * @brief  : 对thread类进行封装，支持线程命名
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-20
 * ======================================================================= */

#include "wthread.h"
#include "func_pub.h"
#include <sstream>
#include "log.h"

namespace gdres {

static thread_local Thread* g_nowThread = nullptr;
static thread_local std::string g_threadName = "NONAME";

Thread* Thread::GetThis()
{
    if (!g_nowThread) {
        g_nowThread = new Thread;  // 如果当前线程为空则创建主线程
    }
    return g_nowThread;
}
const std::string& Thread::GetThreadName() {return g_threadName;}
void Thread::SetThreadName(const std::string& nm)
{
    if (nm.empty() || !g_nowThread) {
        return;
    }
    g_nowThread->name = nm;
    g_threadName = nm;
}

/*==================================================================
 * FuncName : Thread::Thread
 * description : 
 * author : guoshuaiwei  
 *==================================================================*/
Thread::Thread(std::function<void()> cb, const std::string& thName) : id(0), func(cb), thState(INIT)
{
    if (thName.empty()) {
        name = "NONAME";
    }
    name = thName;
    th = new std::thread(&Thread::Run, this);
    sem.wait();
}

Thread::Thread() : name("main"), th(nullptr), thState(EXEC)
{
    id = gdres::GetThreadId();
    g_threadName = name;
}

Thread::~Thread()
{    
    join();
}

/*==================================================================
 * FuncName : Thread::Run
 * description : 线程入口函数
 * author : guoshuaiwei  
 *==================================================================*/
void Thread::Run()
{
    thState = EXEC;
    g_nowThread = this;
    g_threadName = name;
    id = gdres::GetThreadId();

    std::function<void()> cb;
    cb.swap(this->func);

    sem.signal();
    cb();

    thState = TERM;
}

/*==================================================================
 * FuncName : Thread::join
 * description : 线程join方法
 * author : guoshuaiwei  
 *==================================================================*/
void Thread::join()
{
    if (!th) {
        return;
    }
    if (th->joinable()) {
        th->join();
        delete th;
        th = nullptr;
        g_nowThread = nullptr;
    }    
}

/*==================================================================
 * FuncName : Thread::reset
 * description : 当线程结束而对象未被销毁时，可在该对象中重新拉起线程
 * author : guoshuaiwei  
 *==================================================================*/
void Thread::reset(std::function<void()> cb, const std::string& nm)
{
    if (thState != TERM) {
        return;
    }
    delete th;
    func = cb;
    if (nm != "") {
        name = nm;
    }
    th = new std::thread(&Thread::Run, this);
    sem.wait();
}

}

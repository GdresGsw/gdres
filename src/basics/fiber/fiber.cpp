/* =======================================================================
 * @brief  : 封装协程
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-20
 * ======================================================================= */


#include "log.h"
#include "fiber.h"
#include <thread>
#include <atomic>
#include <mutex>


namespace gdres {

static std::mutex fibMtx;

#define FIBER_STACK_SIZE 1024 * 128    // 栈内存默认128k

static std::atomic<uint64_t> g_fibCount(0);
static std::atomic<uint64_t> g_fibId(0);


// 生命周期在线程内的静态变量 当前线程正在运行的协程
static thread_local Fiber* g_mainFiber = nullptr;  // 主协程
static thread_local Fiber* g_nowFiber = nullptr;     // 当前运行协程
// 设置当前线程的运行协程
void Fiber::SetThis(Fiber* fib) { g_nowFiber = fib; }
// 获取当前正在执行的协程
Fiber* Fiber::GetThis()
{
    // 如果当前协程为空，则说明还未创建主协程
    if(!g_nowFiber) {
        g_mainFiber = new Fiber;   
        g_nowFiber = g_mainFiber;
    }
    return g_nowFiber;
}

/*==================================================================
 * FuncName : Fiber::YieldToReady
 * description : 将当前协程切换到后台,并设置为READY状态
 * author : guoshuaiwei  
 *==================================================================*/
void Fiber::YieldToReady()
{
    Fiber* curFib = GetThis();
    curFib->fibState = READY;
    curFib->SwapOut();
}

/*==================================================================
 * FuncName : Fiber::YieldToHold
 * description : 将当前协程切换到后台,并设置为HOLD状态
 * author : guoshuaiwei  
 *==================================================================*/
void Fiber::YieldToHold()
{
    Fiber* curFib = GetThis();
    curFib->fibState = HOLD;
    curFib->SwapOut();
}

// 返回当前协程的总数量
uint64_t Fiber::TotalFibers() { return g_fibCount.load(); }


/*==================================================================
 * FuncName : Fiber::MainFunc
 * description : 协程执行函数执行完成返回到线程主协程
 * author : guoshuaiwei  
 *==================================================================*/
void FIBER_MAINFUNC_CALL Fiber::MainFunc(FIBER_MAINFUNC_PARAMS)
{
    Fiber* curFib = GetThis();
    if (!curFib) {
        return;
    }

    curFib->func();
    curFib->fibState = TERM;
    curFib->SwapOut();
}

/*==================================================================
 * FuncName : Fiber::Fiber
 * description : 返回当前协程的协程ID
 * author : guoshuaiwei  
 *==================================================================*/
uint32_t Fiber::GetFiberId()
{
    if (g_nowFiber) {
        return g_nowFiber->GetId();
    }
    return 0;
}

/*==================================================================
 * FuncName : Fiber::Fiber
 * description : 主协程构造函数
 * author : guoshuaiwei  
 *==================================================================*/
Fiber::Fiber() : fibID(0), fibState(EXEC), stackSize(0)
{
#ifdef _WIN32
    ctx_w = ConvertThreadToFiber(nullptr);
#else
    stack = nullptr;
    getcontext(&ctx);
#endif
    g_fibCount++;
}

// 子协程构造
Fiber::Fiber(std::function<void()> fun, uint32_t stacksize) : fibState(INIT), func(fun)
{
    g_fibCount++;
    {
    std::unique_lock<std::mutex> lck(fibMtx);
    g_fibId++;
    fibID = g_fibId.load();
    }
    stackSize = (stacksize <= 0) ? FIBER_STACK_SIZE : stacksize;
#ifdef _WIN32
    ctx_w = CreateFiber(stackSize, Fiber::MainFunc, NULL);  // 创建纤程
#else
    stack = malloc(stackSize);
    getcontext(&ctx);
    ctx.uc_link = nullptr;
    ctx.uc_stack.ss_sp = stack;
    ctx.uc_stack.ss_size = stackSize;
    makecontext(&ctx, &Fiber::MainFunc, 0);
#endif
} 

Fiber::~Fiber()
{
    g_fibCount--;
#ifdef _WIN32
    if (ctx_w) {
        // DeleteFiber(ctx_w);  // 该函数直接将线程关闭，不再继续执行
    }    
#else
    if (stack) {
        free(stack);
    }
#endif
    // std::cout << fibID << "  count is : " << g_fibCount << std::endl;
    // LOG_SS() << fibID << "  count is : " << g_fibCount;
}

/*==================================================================
 * FuncName : Fiber::Reset
 * description : 重置协程执行函数,并设置状态(当一个协程执行完，分配的内存未释放，进行内存利用)
 * author : guoshuaiwei  
 *==================================================================*/
void Fiber::Reset(std::function<void()> fun)
{
#ifdef _WIN32
    if (fibState != TERM && fibState != EXCEPT && fibState != INIT) {
        return;
    }
    func = fun;
    ctx_w = CreateFiber(stackSize, Fiber::MainFunc, NULL);
#else
    // 栈存在，协程状态为结束或者为初始化时才可以重置
    if (!stack || (fibState != TERM && fibState != EXCEPT && fibState != INIT)) {
        return;
    }
    func = fun;
    getcontext(&ctx);
    ctx.uc_link = nullptr;
    ctx.uc_stack.ss_sp = stack;
    ctx.uc_stack.ss_size = stackSize;

    // 将主函数设置为该协程的回调函数
    makecontext(&ctx, &Fiber::MainFunc, 0);
#endif
    fibState = INIT;
}

/*==================================================================
 * FuncName : Fiber::SwapIn
 * description : 将当前协程切换到运行状态
 * author : guoshuaiwei  
 *==================================================================*/
void Fiber::SwapIn()
{
    if (fibState == EXEC) {
        return;
    }

    SetThis(this); // 将当前协程设置为运行中的协程
    fibState = EXEC;

    // 将当前上下文环境保存进主协程变量，并切换运行环境到当前协程
#ifdef _WIN32
    SwitchToFiber(ctx_w);
#else
    swapcontext(&g_mainFiber->ctx, &ctx);
#endif
}

/*==================================================================
 * FuncName : Fiber::SwapOut
 * description : 把当前协程切换到main协程
 * author : guoshuaiwei  
 *==================================================================*/
void Fiber::SwapOut()
{
    SetThis(g_mainFiber);
#ifdef _WIN32
    SwitchToFiber(g_mainFiber->ctx_w);
#else
    swapcontext(&ctx, &g_mainFiber->ctx);
#endif
}















}






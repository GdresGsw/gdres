/* =======================================================================
 * @brief  : 封装协程
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-20
 * ======================================================================= */
#ifndef _GDRES_FIBER_H_
#define _GDRES_FIBER_H_

#include <stdio.h>
#include <functional>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>  // windows下用纤程实现协程
#else
#include <ucontext.h>
#endif

#ifdef _WIN32
#define FIBER_MAINFUNC_PARAMS LPVOID lpParameter
#define FIBER_MAINFUNC_CALL __stdcall
#else
#define FIBER_MAINFUNC_PARAMS
#define FIBER_MAINFUNC_CALL
#endif


namespace gdres {


class Fiber 
{
friend class Scheduler;  // 调度器类
public:
    // 协程状态
    enum State {        
        INIT,    // 初始化状态        
        HOLD,    // 暂停状态
        EXEC,    // 执行中状态
        TERM,    // 结束状态
        READY,   // 可执行状态
        EXCEPT   // 异常状态
    };

    Fiber(std::function<void()> fun, uint32_t stacksize = 0);
    ~Fiber();

    void Reset(std::function<void()> fun);       // 重置协程执行函数,并设置状态
    void SwapIn();                               // 将当前协程切换到运行状态(唤醒当前协程)
    void SwapOut();                              // 将当前协程切换到后台（挂起当前协程)


    uint32_t GetId() const { return fibID;}     // 返回协程id
    State GetState() const { return fibState;}  // 返回协程状态

    static void SetThis(Fiber*);                // 设置当前线程的运行协程
    static Fiber* GetThis();                    // 返回当前所在的协程,无协程时创建主协程
    static void YieldToReady();                 // 将当前协程切换到后台,并设置为READY状态
    static void YieldToHold();                  // 将当前协程切换到后台,并设置为HOLD状态
    static uint64_t TotalFibers();              // 返回当前协程的总数量
    static void FIBER_MAINFUNC_CALL MainFunc(FIBER_MAINFUNC_PARAMS);   // 协程执行函数执行完成返回到线程主协程
    // static void CallerMainFunc();               // 执行完成返回到线程调度协程
    static uint32_t GetFiberId();               // 获取当前协程的id

private:
    Fiber();    // 无参构造   主协程构造

private:
    uint32_t fibID;                     // 协程id    
    State fibState;                     // 协程状态       
    uint32_t stackSize;                 // 协程运行栈大小
    std::function<void()> func;         // 协程运行函数
#ifdef _WIN32
    LPVOID ctx_w;
#else
    ucontext_t ctx;                     // 协程上下文
    void* stack;                        // 协程运行栈指针
#endif
};








}






#endif


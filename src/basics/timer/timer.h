/* =======================================================================
 * @brief  : 封装计时器
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */
#ifndef _GDRES_TIMER_H_
#define _GDRES_TIMER_H_

#include <thread>
#include <functional>
#include <vector>
#include <set>
#include "lock.h"

#define SLEEPS(x) std::this_thread::sleep_for(std::chrono::seconds(x))
#define SLEEPMS(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

namespace gdres {

class TimerMgr;

// 定时器
class Timer
{
friend class TimerMgr;
public:
    bool Cancel();
    bool Refresh();
    bool Reset(uint64_t interval, bool fromNow);

private:
    Timer(uint64_t interval, std::function<void()> cb, bool recurring, TimerMgr* mgr);
    Timer(uint64_t occurTime);

private:
    TimerMgr* m_manager;                        // 管理器
    bool m_recurring;                           // 是否循环
    std::function<void()> m_func;               // 执行函数
    uint64_t m_cycle;                           // 间隔多长时间执行
    uint64_t m_occur;                           // 具体执行时间点

private:
    // 定时器比较方法
    struct Comparator {
        bool operator()(const Timer*& lhs, const Timer*& rhs) const;
    };
};

// 定时器管理器
class TimerMgr
{
friend class Timer;
public:
    TimerMgr() {}
    virtual ~TimerMgr() {}

    Timer* AddTimer(uint64_t interval, std::function<void()> cb,
                    bool recurring = true);                             // 添加定时器

    void GetCbList(std::vector<std::function<void()>>& cbs);            // 获取需要执行的定时器的回调函数列表

private:
    RdWrLock m_wrMtx;                  // 读写锁
    std::set<Timer*> m_timers;         // 定时器集合
};




}

#endif

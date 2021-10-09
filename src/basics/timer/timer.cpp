/* =======================================================================
 * @brief  : 计时器
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */

#include "timer.h"
#include "func_pub.h"
#include "log.h"

namespace gdres {

/*==================================================================
 * FuncName : Timer::Timer
 * description :  定时器
 * author : guoshuaiwei  
 *==================================================================*/
Timer::Timer(uint64_t interval, std::function<void()> cb, bool recur, TimerMgr* mgr) :
    m_manager(mgr), m_recurring(recur), m_func(cb), m_cycle(interval)
{
    m_occur = GetNowTimMS() + interval;
}

Timer::Timer(uint64_t occurTime) : m_manager(nullptr), m_recurring(0), m_func(nullptr), m_cycle(false)
{
    m_occur = occurTime;
}

/*==================================================================
 * FuncName : Timer::Cancel
 * description :  取消定时器，即将定时器从管理器中删除
 * author : guoshuaiwei  
 *==================================================================*/
bool Timer::Cancel()
{
    return true;
}

/*==================================================================
 * FuncName : Timer::Refresh
 * description :  刷新定时器，即定时器重新开始计时
 * author : guoshuaiwei  
 *==================================================================*/
bool Timer::Refresh()
{
    return true;
}

/*==================================================================
 * FuncName : Timer::Reset
 * description :  重置定时器，重新设置定时器的时间周期
 * @param[in] interval 时间间隔，fromNow 是否重新开始计时
 * author : guoshuaiwei  
 *==================================================================*/
bool Reset(uint64_t interval, bool fromNow)
{
    return true;
}

/*==================================================================
 * FuncName : TimerMgr::AddTimer
 * description :  添加定时器，决定定时器在管理器队列中排序的方法
 * @param[in] interval 时间间隔，cb 回调函数，recurring 是否循环定时
 * author : guoshuaiwei  
 *==================================================================*/
bool Timer::Comparator::operator()(const Timer*& lhs, const Timer*& rhs) const
{
    if(lhs->m_cycle < rhs->m_cycle) {
        return true;
    }
    if(rhs->m_cycle < lhs->m_cycle) {
        return false;
    }
    return lhs < rhs;
}


/*==================================================================
 * FuncName : TimerMgr::AddTimer
 * description :  添加定时器
 * @param[in] interval 时间间隔，cb 回调函数，recurring 是否循环定时
 * author : guoshuaiwei  
 *==================================================================*/
Timer* TimerMgr::AddTimer(uint64_t interval, std::function<void()> cb, bool recurring)
{
    Timer* timer = new Timer(interval, cb, recurring, this);
    m_wrMtx.wrlock();
    m_timers.insert(timer);
    m_wrMtx.wrunlock();

    return timer;
}

/*==================================================================
 * FuncName : TimerMgr::GetCbList
 * description :  获取需要执行的定时器的回调函数的列表
 * @param[in] cbs 需要执行的回调函数列表,如果有需要处理的定时器则返回true
 * author : guoshuaiwei  
 *==================================================================*/
void TimerMgr::GetCbList(std::vector<std::function<void()>>& cbs)
{
    std::vector<Timer*> temp;
    m_wrMtx.wrlock();
    if (m_timers.empty()) {
        m_wrMtx.wrunlock();
        return;
    }

    uint64_t nowMs = GetNowTimMS();

    // TODO : 此处查找方法可优化
    auto it = m_timers.begin();
    while (it != m_timers.end() && (*it)->m_occur <= nowMs) {
        it++;
    }

    temp.insert(temp.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(temp.size());

    for(auto& timer : temp) {
        cbs.push_back(timer->m_func);
        if(timer->m_recurring) {
            timer->m_occur = nowMs + timer->m_cycle;
            m_timers.insert(timer);
        } else {
            timer->m_func = nullptr;
            delete timer;
        }
    }
    m_wrMtx.wrunlock();
    return;
}














}

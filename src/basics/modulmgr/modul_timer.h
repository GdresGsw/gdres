/* =======================================================================
 * @brief  : 模块管理定时器
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "timer.h"
#include "thread_pool.h"
#include "wthread.h"

namespace gdres {

#define TIMER_MGR_THREAD_NUM 2     // 默认定义2个工作线程
#define TIMER_PRECISION 100        // 默认精度100ms

// 模块定时器管理
class ModulTimerMgr : public TimerMgr
{
public:
    ModulTimerMgr();
    ~ModulTimerMgr() {Stop();}

    void Init();
    void Stop();

private:
    void Run();             // 定时器线程函数

private:
    ThreadPool* m_pool;        // 线程池
    uint16_t m_threadCnt;      // 线程池工作线程数量

    uint16_t m_precis;         // 定时器精度
    bool m_stopping;           // 定时器停止标志
};







}
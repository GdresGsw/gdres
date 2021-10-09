/* =======================================================================
 * @brief  : 模块管理定时器
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "modul_timer.h"
#include "log.h"


namespace gdres {

/*==================================================================
 * FuncName : ModulTimerMgr::ModulTimerMgr
 * description :  
 * author : guoshuaiwei  
 *==================================================================*/
ModulTimerMgr::ModulTimerMgr() : m_pool(nullptr), m_threadCnt(TIMER_MGR_THREAD_NUM),
                                 m_precis(TIMER_PRECISION), m_stopping(false)
{
    Init();
}

/*==================================================================
 * FuncName : ModulTimerMgr::Init
 * description :  定时器管理初始化
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTimerMgr::Init()
{
    m_pool = new ThreadPool(m_threadCnt + 1, "pool");
    SLEEPMS(500);
    m_pool->AddTask(std::bind(&ModulTimerMgr::Run, this));
}

/*==================================================================
 * FuncName : ModulTimerMgr::Run
 * description :  定时器管理线程函数
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTimerMgr::Run()
{
    LOG_SS() << "modultimermgr thread run!";
    std::vector<std::function<void()>> cbs;
    while (!m_stopping) {
        cbs.clear();
        SLEEPMS(m_precis);
        GetCbList(cbs);
        if (!cbs.empty()) {
            for (auto &it : cbs) {
                m_pool->AddTask(it);
            }
        }        
    }
    return;
}

/*==================================================================
 * FuncName : ModulTimerMgr::Stop
 * description :  定时器管理停止工作
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTimerMgr::Stop()
{
    m_stopping = true;

    if (m_pool) {
        delete m_pool;
    }   
}













}

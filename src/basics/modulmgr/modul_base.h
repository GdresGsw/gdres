/* =======================================================================
 * @brief  : 业务模块基类
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#ifndef _GDRES_MODUL_BASE_H_
#define _GDRES_MODUL_BASE_H_

#include "lock.h"
#include "modul_mgr.h"
#include "msgqueue.h"
#include <string>

namespace gdres {


class ModulBase
{
public:
    ModulBase(uint32_t modID, const std::string& nm = "");
    virtual ~ModulBase() {Stop();}

    int Init();
    void Run();

    virtual void WakeUp();        
    virtual int MsgPush(GDRES_MSG* msg);            // 接收消息的接口
    virtual void Stop();

    virtual void ProcessMsg(GDRES_MSG*) = 0;         // 处理消息
    virtual void TimerEvent() = 0;                   // 定时事件接口


protected:
    virtual GDRES_MSG* TakeMsg();                    // 从消息队列中取消息

protected:
    uint32_t m_modId;             // 模块ID 模块的唯一标识
    std::string m_name;           // 模块名字
    MsgQueue* m_msgQ;              // 消息队列

    size_t m_timerCnt;            // 时钟计数
    uint16_t m_interval;          // 默认定时器周期
    ModulMgr* m_modulmgr;         // 所属模块管理类

private:
    Semaphore m_sem;              // 信号量
    bool m_stopping;              // 停止标记    
    
};




}
#endif



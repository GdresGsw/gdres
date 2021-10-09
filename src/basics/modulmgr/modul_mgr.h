/* =======================================================================
 * @brief  : 模块管理
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2020/9/20
 * ======================================================================= */
#ifndef _GDRES_MODUL_MANAGER_H_
#define _GDRES_MODUL_MANAGER_H_

#include "log.h"
#include <vector>
#include <queue>
#include "wthread.h"
#include "msgdata.h"
#include <unordered_map>
#include "msgqueue.h"
#include "modul_timer.h"

namespace gdres {

// 消息路由
// class Exchange



// 模块注册信息
struct RegisterInfo
{
    uint32_t modId;                                       // 模块ID
    std::string name;                                     // 模块名

    std::function<void()> runFunc;                        // 模块线程入口函数
    std::function<int(GDRES_MSG*)> notifyItf;             // 给模块投递消息的接口

    std::function<void()> timerItf;                       // 模块定时事件的接口，如果为空则无定时事件
    uint16_t interval;                                    // 定时器执行周期
    bool recurring;                                       // 定时器是否循环执行
};

struct TimerInfo
{
    std::function<void()> timerItf;
    uint16_t interval;
    bool recurring;
};

// 注册的模块信息结构体
struct ModInfo {
    RegisterInfo regInfo;                 // 模块注册信息
    Thread* th;                           // 模块运行线程

    ModInfo() : regInfo({0}), th(nullptr) {}
};

class ModulMgr
{
public:
    typedef gdres::RdWrLock MutexType;

    int Register(RegisterInfo);                          // 模块信息注册    
    void UnRegister(uint32_t modID);                     // 注销模块
    bool FindModulId(uint32_t modID);                    // 查找模块id是否被注册

    int PostMsg(uint32_t desID, GDRES_MSG* sendMsg);                            // 模块发消息
    int PostMsg(const std::vector<uint32_t>& ids, GDRES_MSG* sendMsg);          // 向多个模块发消息
    int PostAndRecv(uint32_t desID, GDRES_MSG* sendMsg, GDRES_MSG*& recvMsg);   // 发送即时消息

    Timer* RegisTimer(uint32_t modID, TimerInfo info);           // 注册模块定时器(供模块注册临时定时器使用)
    // int UnRegisTimer(uint32_t modID);                            // 注销模块临时定时器

    static ModulMgr* GetThis();

private:    
    ModulMgr();
    

private:
    std::unordered_map<uint32_t, ModInfo*> m_mods;
    MutexType m_wrMtx;
    ModulTimerMgr* m_timerMgr;
};







}


#endif




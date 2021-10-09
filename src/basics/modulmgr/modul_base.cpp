/* =======================================================================
 * @brief  : 业务模块基类
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "modul_base.h"

namespace gdres {


/*==================================================================
 * FuncName : ModulBase::ModulBase
 * description :  
 * author : guoshuaiwei  
 *==================================================================*/
ModulBase::ModulBase(uint32_t modID,  const std::string& nm) : m_modId(modID), m_name(nm), 
                                      m_timerCnt(0), m_interval(SECOND_TIMER), m_stopping(false)
{
    m_modulmgr = ModulMgr::GetThis();
    m_msgQ = new MsgQueue();
}

/*==================================================================
 * FuncName : ModulBase::Init
 * description :  模块初始化，向模块管理进行注册
 * author : guoshuaiwei  
 *==================================================================*/
int ModulBase::Init()
{
    RegisterInfo info = {0};
    info.modId = m_modId;
    info.name = m_name;
    info.runFunc = std::bind(&ModulBase::Run, this);
    info.notifyItf = std::bind(&ModulBase::MsgPush, this, std::placeholders::_1);
    info.timerItf = std::bind(&ModulBase::TimerEvent, this);
    info.interval = m_interval;
    info.recurring = true;

    int ret = m_modulmgr->Register(info);
    if (ret != MSG_OK) {
        m_stopping = true;
        return MSG_ERR;
    }

    return MSG_OK;
}

/*==================================================================
 * FuncName : ModulBase::Run
 * description :  模块入口函数
 * author : guoshuaiwei  
 *==================================================================*/
void ModulBase::Run()
{
    GDRES_LOG(LOG_EVENT, "modul(%d) running!", m_modId);
    while (!m_stopping) {
        m_sem.wait();
        GDRES_MSG* recvMsg = TakeMsg();
        if (recvMsg) {
            ProcessMsg(recvMsg);
        }
    }

    GDRES_LOG(LOG_EVENT, "modul(%d) stopping!", m_modId);
    return;
}

/*==================================================================
 * FuncName : ModulBase::TakeMsg
 * description :  从消息队列中取消息
 * author : guoshuaiwei  
 *==================================================================*/
GDRES_MSG* ModulBase::TakeMsg()
{
    return m_msgQ->take();
}

/*==================================================================
 * FuncName : ModulBase::MsgPush
 * description :  将消息放入消息队列中
 * author : guoshuaiwei  
 *==================================================================*/
int ModulBase::MsgPush(GDRES_MSG* msg)
{
    int ret = m_msgQ->push(msg);
    if (ret != MSG_ERR) {
        WakeUp();
    }

    return ret;
}

/*==================================================================
 * FuncName : ModulBase::WakeUp
 * description :  唤醒模块处理业务
 * author : guoshuaiwei  
 *==================================================================*/
void ModulBase::WakeUp()
{
    m_sem.signal();
}

/*==================================================================
 * FuncName : ModulBase::Stop
 * description :  关闭模块
 * author : guoshuaiwei  
 *==================================================================*/
void ModulBase::Stop()
{
    if (m_stopping) {
        return;
    }
    m_stopping = true;
    WakeUp();
    // 注销模块（逻辑上模块不能在自己的线程里注销自己，可以发消息给管理模块进行注销，以后优化）
    // m_modulmgr->UnRegister(m_modId);
    delete m_msgQ;
    return;    
}



}



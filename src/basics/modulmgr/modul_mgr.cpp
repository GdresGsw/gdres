/* =======================================================================
 * @brief  : 消息管理
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "modul_mgr.h"

namespace gdres {

static ModulMgr* g_modulMgr = nullptr;


// 获取模块管理类的指针
ModulMgr* ModulMgr::GetThis()
{
    if (!g_modulMgr) {
        g_modulMgr = new ModulMgr;
    }
    return g_modulMgr;
}

/*==================================================================
 * FuncName : ModulMgr::ModulMgr
 * description :  
 * author : guoshuaiwei  
 *==================================================================*/
ModulMgr::ModulMgr()
{
    m_timerMgr = new ModulTimerMgr;
}

/*==================================================================
 * FuncName : ModulMgr::Register
 * description :  注册模块信息
 * author : guoshuaiwei  
 *==================================================================*/
int ModulMgr::Register(RegisterInfo info)
{
    if (info.modId > MODUL_ID_MAX_VALU || !info.runFunc || !info.notifyItf) {
        return MSG_ERR;
    }

    if (m_mods.count(info.modId) != 0) {
        GDRES_LOG(LOG_ERROR, "modul(%d) register fail : modul ID conflict!", info.modId);
        return MSG_ERR;
    }

    // 拉起模块线程
    ModInfo* modInfo = new ModInfo;
    modInfo->regInfo = info;
    modInfo->th = new Thread(info.runFunc, info.name);

    m_wrMtx.wrlock();
    m_mods.emplace(info.modId, modInfo);
    m_wrMtx.wrunlock();

    // 为模块添加定时器
    if (info.timerItf) {
        m_timerMgr->AddTimer(info.interval, info.timerItf, info.recurring);
    }

    GDRES_LOG(LOG_EVENT, "modul(%d) register OK!", info.modId);
    return MSG_OK;
}

/*==================================================================
 * FuncName : ModulMgr::UnRegister
 * description :  注销模块信息 
 * author : guoshuaiwei  
 *==================================================================*/
void ModulMgr::UnRegister(uint32_t modID)
{
    if (!FindModulId(modID)) {
        return;
    }
    
    delete m_mods[modID]->th;   // 先退出模块线程
    delete m_mods[modID];       // 再删除模块信息

    m_wrMtx.wrlock();
    m_mods.erase(modID);
    m_wrMtx.wrunlock();

    return;
}

/*==================================================================
 * FuncName : ModulMgr::PostMsg
 * description :  向目标模块发送消息
 * author : guoshuaiwei  
 *==================================================================*/
int ModulMgr::PostMsg(uint32_t desID, GDRES_MSG* sendMsg)
{
    if (!FindModulId(desID)) {
        return MSG_ERR;
    }

    int ret = m_mods[desID]->regInfo.notifyItf(sendMsg);  // 将消息放入目标模块的消息队列中
    return ret;
}

/*==================================================================
 * FuncName : ModulMgr::PostMsg
 * description :  向多模块发送消息（如果有一个id查找失败则全部发送失败）
 * author : guoshuaiwei  
 *==================================================================*/
int ModulMgr::PostMsg(const std::vector<uint32_t>& ids, GDRES_MSG* sendMsg)
{
    bool allow = false;
    m_wrMtx.rdlock();
    for (size_t i = 0; i < ids.size(); ++i) {
        if (m_mods.find(ids[i]) == m_mods.end()) {
            return MSG_ERR;
        }
    }
    m_wrMtx.rdunlock();

    int ret = 0;
    for (auto &it : ids) {
        GDRES_MSG* msg = new GDRES_MSG;
        memcpy(msg, sendMsg, sendMsg->msgLen);
        if (PostMsg(it, msg) != MSG_OK) {
            ret = MSG_ERR;
        };
    }
    
    delete sendMsg;
    return ret;    
}

/*==================================================================
 * FuncName : ModulMgr::RegisTimer
 * description :  注册模块定时器（区别于注册模块信息时的默认定时器）
 * author : guoshuaiwei  
 *==================================================================*/
Timer* ModulMgr::RegisTimer(uint32_t modID, TimerInfo info)
{
    if (modID > MODUL_ID_MAX_VALU || !info.timerItf) {
        return nullptr;
    }

    if (!FindModulId(modID)) {
        return nullptr;
    }

    // 如果模块存在则直接添加定时器
    Timer* timer = m_timerMgr->AddTimer(info.interval, info.timerItf, info.recurring);

    GDRES_LOG(LOG_EVENT, "modul(%d) register timer OK!", modID);
    return timer;
}

/*==================================================================
 * FuncName : ModulMgr::PostAndRecv
 * description : 发送即时消息
 * author : guoshuaiwei  
 *==================================================================*/
int ModulMgr::PostAndRecv(uint32_t desID, GDRES_MSG* sendMsg, GDRES_MSG*& recvMsg)
{
    if (!FindModulId(desID)) {
        return MSG_ERR;
    }

    // TODO : 超时机制，消息序号匹配机制
    return MSG_OK;
}

/*==================================================================
 * FuncName : ModulMgr::FindModulId
 * description : 查找模块ID是否被注册
 * author : guoshuaiwei  
 *==================================================================*/
bool ModulMgr::FindModulId(uint32_t modID)
{
    m_wrMtx.rdlock();
    if (m_mods.find(modID) == m_mods.end()) {
        m_wrMtx.rdunlock();
        return false;
    }

    m_wrMtx.rdunlock();
    return true;
}





}
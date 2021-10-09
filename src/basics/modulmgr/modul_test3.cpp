/* =======================================================================
 * @brief  : 测试业务模块3
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "modul_mgr.h"
#include "timer.h"
#include "log.h"
#include "modul_test.h"

namespace gdres {

static ModulTest3* g_modulTest = nullptr;

ModulTest3* ModulTest3::GetThis()
{
    if (!g_modulTest) {
        g_modulTest = new ModulTest3(MODUL_TES3, "test_3");
    }
    return g_modulTest;
}

/*==================================================================
 * FuncName : ModulTest3::ModulTest3
 * description :  构造
 * author : guoshuaiwei  
 *==================================================================*/
ModulTest3::ModulTest3(uint32_t modID, const std::string& nm) : ModulBase(modID, nm), msgCnt(0)
{
    g_modulTest = this;
    Init();
}

/*==================================================================
 * FuncName : ModulTest3::ProcessMsg
 * description :  处理消息
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTest3::ProcessMsg(GDRES_MSG* recvMsg)
{
    switch (recvMsg->msgType)
    {
    case TEST3_MSG_TYPE1:
        msgCnt++;
        delete recvMsg;
        break;
    
    case TEST3_MSG_TYPE2:
        break;

    case TEST3_MSG_TYPE3:
        // LOG_SS() << "recv test msg : test1->test3";
        msgCnt++;
        recvMsg->srcID = MODUL_TES3;
        recvMsg->msgType = TEST2_MSG_TYPE3;
        m_modulmgr->PostMsg(MODUL_TES2, recvMsg);
        break;
    
    default:
        break;
    }
}


/*==================================================================
 * FuncName : ModulTest3::TimerEvent
 * description :  定时事件
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTest3::TimerEvent()
{
    static bool tigger = 0;
    // 触发一次三个模块之间的循环消息
    if (tigger) {
        tigger = false;
        GDRES_MSG* msg = new GDRES_MSG;
        msg->srcID = MODUL_TES3;
        msg->level = GENERAL;
        msg->msgType = TEST2_MSG_TYPE3;
        m_modulmgr->PostMsg(MODUL_TES2, msg);
    }

    GDRES_MSG* msg1 = new GDRES_MSG;
    msg1->srcID = MODUL_TES3;
    msg1->level = GENERAL;
    msg1->msgType = TEST1_MSG_TYPE1;
    m_modulmgr->PostMsg(MODUL_TES1, msg1);

    GDRES_LOG(LOG_EVENT, "test modul 3 recv msg cnt : %d", msgCnt.load());
    msgCnt = 0;
}















}

/* =======================================================================
 * @brief  : 测试业务模块1
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "modul_mgr.h"
#include "timer.h"
#include "log.h"
#include "modul_test.h"


namespace gdres {

static ModulTest1* g_modulTest1 = nullptr;

ModulTest1* ModulTest1::GetThis()
{
    if (!g_modulTest1) {
        g_modulTest1 = new ModulTest1(MODUL_TES1, "test_1");
    }
    return g_modulTest1;
}

/*==================================================================
 * FuncName : ModulTest1::ModulTest1
 * description :  构造
 * author : guoshuaiwei  
 *==================================================================*/
ModulTest1::ModulTest1(uint32_t modID, const std::string& nm) : ModulBase(modID, nm), msgCnt(0)
{
    g_modulTest1 = this;
    Init();
}

/*==================================================================
 * FuncName : ModulTest1::ProcessMsg
 * description :  处理消息
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTest1::ProcessMsg(GDRES_MSG* recvMsg)
{
    switch (recvMsg->msgType)
    {
    case TEST1_MSG_TYPE1:
        msgCnt++;
        delete recvMsg;
        break;
    
    case TEST1_MSG_TYPE2:
        break;

    case TEST1_MSG_TYPE3:
        // LOG_SS() << "recv test msg : test2->test1";
        msgCnt++;
        recvMsg->srcID = MODUL_TES1;
        recvMsg->msgType = TEST3_MSG_TYPE3;
        m_modulmgr->PostMsg(MODUL_TES3, recvMsg);
        break;
    
    default:
        break;
    }
}

/*==================================================================
 * FuncName : ModulTest1::TimerEvent
 * description :  定时事件
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTest1::TimerEvent()
{
    // 模块1每2秒向模块2发送一条TEST2_MSG_TYPE2消息
    if (++m_timerCnt >= 2) {
        m_timerCnt = 0;
        GDRES_MSG* msg = new GDRES_MSG;
        msg->srcID = MODUL_TES1;
        msg->level = GENERAL;
        msg->msgType = TEST2_MSG_TYPE2;
        m_modulmgr->PostMsg(MODUL_TES2, msg);
    }

    GDRES_LOG(LOG_EVENT, "test modul 1 recv msg cnt : %d", msgCnt.load());
    msgCnt = 0;
}















}

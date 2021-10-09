/* =======================================================================
 * @brief  : 测试业务模块2
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */
#include "modul_mgr.h"
#include "timer.h"
#include "log.h"
#include "modul_test.h"

namespace gdres {

static ModulTest2* g_modulTest = nullptr;

ModulTest2* ModulTest2::GetThis()
{
    if (!g_modulTest) {
        g_modulTest = new ModulTest2(MODUL_TES2, "test_2");
    }
    return g_modulTest;
}

/*==================================================================
 * FuncName : ModulTest2::ModulTest2
 * description :  构造
 * author : guoshuaiwei  
 *==================================================================*/
ModulTest2::ModulTest2(uint32_t modID, const std::string& nm) : ModulBase(modID, nm), msgCnt(0)
{
    g_modulTest = this;
    Init();
}

/*==================================================================
 * FuncName : ModulTest2::ProcessMsg
 * description :  处理消息
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTest2::ProcessMsg(GDRES_MSG* recvMsg)
{
    switch (recvMsg->msgType)
    {
    case TEST2_MSG_TYPE1:
        msgCnt++;
        // LOG_SS() << "recv test2 msg : test2->test2";
        delete recvMsg;
        break;
    
    case TEST2_MSG_TYPE2:
        msgCnt++;
        // LOG_SS() << "recv test1 msg : test1->test2";
        delete recvMsg;
        break;

    case TEST2_MSG_TYPE3:
        // LOG_SS() << "recv test msg : test3->test2";
        msgCnt++;
        recvMsg->srcID = MODUL_TES2;
        recvMsg->msgType = TEST1_MSG_TYPE3;
        m_modulmgr->PostMsg(MODUL_TES1, recvMsg);
        break;
    
    default:
        break;
    }
}

/*==================================================================
 * FuncName : ModulTest2::TimerEvent
 * description :  定时事件
 * author : guoshuaiwei  
 *==================================================================*/
void ModulTest2::TimerEvent()
{
    // 每秒发给自己一条TEST2_MSG_TYPE1消息给MODUL_TES3发一条TEST3_MSG_TYPE1消息
    GDRES_MSG* msg1 = new GDRES_MSG;
    msg1->srcID = MODUL_TES2;
    msg1->level = GENERAL;
    msg1->msgType = TEST2_MSG_TYPE1;
    m_modulmgr->PostMsg(MODUL_TES2, msg1);

    GDRES_MSG* msg2 = new GDRES_MSG;
    msg2->srcID = MODUL_TES2;
    msg2->level = GENERAL;
    msg2->msgType = TEST3_MSG_TYPE1;
    m_modulmgr->PostMsg(MODUL_TES3, msg2);



    GDRES_LOG(LOG_EVENT, "test modul 2 recv msg cnt : %d", msgCnt.load());
    msgCnt = 0;
}















}

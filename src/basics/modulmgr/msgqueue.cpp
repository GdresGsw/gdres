/* =======================================================================
 * @brief  : 消息队列类
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */

#include "msgqueue.h"


namespace gdres {

/*==================================================================
 * FuncName : MsgQueue::MsgQueue
 * description :  
 * author : guoshuaiwei  
 *==================================================================*/
MsgQueue::MsgQueue(uint32_t size) : m_imMsg(nullptr), m_capacity(size), m_size(0)
{
}

/*==================================================================
 * FuncName : MsgQueue::MsgQueue
 * description :  向队列中添加消息：成功返回MSG_OK，失败返回MSG_ERR(说明消息队列满了)
 * author : guoshuaiwei  
 *==================================================================*/
int MsgQueue::push(GDRES_MSG* msg)
{
    if ((msg == nullptr) || (m_size >= m_capacity)) {
        return MSG_ERR;
    }
    std::unique_lock<std::mutex> lck(m_mtx);
    switch (msg->level)
    {
    case GENERAL:
        m_genaralMsgs.push(msg);
        break;
    
    case IMPORTANCE:
        m_importantMsgs.push(msg);
        break;
    
    case IMMEDIATE:
        if (m_imMsg) {
            return MSG_ERR;
        }
        m_imMsg = msg;
        break;    
    
    default:
        return MSG_ERR;
    }

    m_size++;
    return MSG_OK;
}

/*==================================================================
 * FuncName : MsgQueue::MsgQueue
 * description :  从队列中取出消息
 * author : guoshuaiwei  
 *==================================================================*/
GDRES_MSG* MsgQueue::take()
{
    if (m_size == 0) {
        return nullptr;
    }

    GDRES_MSG* msg = nullptr;
    std::unique_lock<std::mutex> lck(m_mtx);
    if (m_imMsg) {
        msg = m_imMsg;
        m_imMsg = nullptr;
    } else if (!m_importantMsgs.empty()) {
        msg = m_importantMsgs.front();
        m_importantMsgs.pop();
    } else if (!m_genaralMsgs.empty()) {
        msg = m_genaralMsgs.front();
        m_genaralMsgs.pop();
    }

    m_size--;
    return msg;
}

}



/* =======================================================================
 * @brief  : 消息队列类
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-08-10
 * ======================================================================= */
#ifndef _GDRES_MSG_QUEUE_H_
#define _GDRES_MSG_QUEUE_H_

#include "log.h"
#include <vector>
#include <queue>
#include "msgdata.h"

namespace gdres {


class MsgQueue
{
public:
    MsgQueue(uint32_t size = MSG_QUEUE_CAPACITY);
    ~MsgQueue() {}

    int push(GDRES_MSG*);      // 将消息放入消息队列
    GDRES_MSG* take();         // 从消息队列中取消息

private:
    std::mutex m_mtx;

    // 暂时消息级别较少，所以每个消息级别维护一个消息队列
    std::queue<GDRES_MSG*> m_genaralMsgs;              // 一般级别消息的消息队列
    std::queue<GDRES_MSG*> m_importantMsgs;            // 重要级别消息的消息队列
    GDRES_MSG* m_imMsg;                                // 即时消息

    uint32_t m_capacity;                               // 消息队列容量
    std::atomic<uint32_t> m_size;                      // 当前消息队列中的消息总数量       
};







}



#endif

/* =======================================================================
 * @brief  : 定义消息系统用到的一些数据结构
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-08-10
 * ======================================================================= */
#ifndef _GDRES_MSG_DATA_H_
#define _GDRES_MSG_DATA_H_

#include <stdio.h>
#include <stdint.h>

namespace gdres {

#define MSG_ERR -1
#define MSG_OK 0

#define MSG_QUEUE_CAPACITY 100            // 消息队列默认容量
#define MODUL_ID_MAX_VALU  0x20          // 模块ID有效值范围            

#define SECOND_TIMER 1000  // 秒定时器

#define MSG_LEVEL_CNT 3   // 消息级别数量
// 消息级别
enum MsgLevel {
    GENERAL = 0,        // 一般消息
    IMPORTANCE = 1,     // 重要消息
    IMMEDIATE = 2,      // 即时消息，发送方会一直等待消息返回再处理其他业务，不会同时出现2个以上的即时消息
};

// 消息结构体
typedef struct GdersMsg {
    uint32_t srcID;   // 源模块ID
    uint32_t desID;   // 目标模块ID
    size_t msgType;   // 消息类型
    MsgLevel level;   // 消息级别
    size_t seqNo;     // 消息序列号
    size_t msgLen;    // 消息长度
    char* content;    // 消息内容
} GDRES_MSG;


// 模块ID信息
enum MODUL_ID {
    MODUL_TES1 = 0x01,
    MODUL_TES2 = 0x02,
    MODUL_TES3 = 0x03,
};









}


#endif

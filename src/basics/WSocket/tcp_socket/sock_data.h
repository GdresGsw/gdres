#ifndef TCP_SOCKET_DATA_H
#define TCP_SOCKET_DATA_H
#include <sock_p.h>

#define IP_ADDR_LEN_MAX 20

#define VOS_ERR 1
#define VOS_OK 0

#define VOS_TRUE 1
#define VOS_FALSE 0





#define IPV4 AF_INET
#define IPV6 AF_INET6

#define RECV_BUFF_SIZE 10240 * 5  // 接收消息缓冲区50K
#define SEND_BUFF_SIZE 10240 * 5  // 发送消息缓冲区50K



enum LOG_CMD 
{
    LOG_IN = 0,
    LOG_RET,
    LOG_OTHER,
};


// 视频中的继承写法
struct DataHead
{
    int dataLength;
    int cmd;
};

struct LogIn : public DataHead
{
    LogIn()
    {
        dataLength = sizeof(LogIn);
        cmd = LOG_IN;
    }

    int age;
    char name[32];
    char data[1008];
};

struct LogInRet : public DataHead
{
    LogInRet()
    {
        dataLength = sizeof(LogInRet);
        cmd = LOG_RET;
        ret = 0;
    }

    int ret;
    char data[1012];
};

struct LogInOther : public DataHead
{
    LogInOther()
    {
        dataLength = sizeof(LogInRet);
        cmd = LOG_OTHER;
        sock = INVALID_SOCKET;
    }

    SOCKET sock;
};
























#endif




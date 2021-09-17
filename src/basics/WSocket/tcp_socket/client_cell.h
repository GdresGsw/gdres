#ifndef _TCP_SERVER_CLIENT_CELL_
#define _TCP_SERVER_CLIENT_CELL_

#include <sock_p.h>
#include <sock_data.h>
#include <INetEvent.h>
#include <atomic>


// 服务器客户端列表中维护的客户端基类
class ClientCell
{
public:
    ClientCell(SOCKET sock, sockaddr_in sAddr, INetEvent* eve);
    virtual ~ClientCell();

    SOCKET GetSock() {return cSockfd;}
    char* GetClientAddr() {return inet_ntoa(cSockAddr.sin_addr);}

    virtual bool SendMsg(DataHead* sendMsg); // 发送消息
    virtual bool RecvMsg(); // 接收并处理消息  
    virtual void Close(); // 关闭socket
    virtual bool IsRun(); // 是否运行
protected:
    virtual void PrcMsg();  // 处理消息

private:
    SOCKET cSockfd;
    sockaddr_in cSockAddr;

    char recvMsgBuff[RECV_BUFF_SIZE]; // 接收到的消息先拷贝进消息缓冲区，再通过接收缓冲区取消息
    int recvLastPos; // 记录当前消息缓冲区中消息的长度

    char sendMsgBuff[SEND_BUFF_SIZE]; // 发送的数据先放到缓冲区再定时或定量发出去，缓解send的调用对性能的影响
    int sendLastPos; // 记录当前消息缓冲区中消息的长度

    INetEvent* event;

public:
    static std::atomic_int recvCnt;  // 统计recv函数调用次数
    static std::atomic_int packCnt;  // 统计接收的数据包的个数
    static std::atomic_int sendCnt;  // 统计send函数调用次数
    static std::atomic_int clientCnt; // 接入的客户端的数量

};



#endif


#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <sock_p.h>
#include <sock_data.h>

class WTcpClient
{
public:
    WTcpClient();
    ~WTcpClient() {CloseSocket();}

    // 初始化socket
    int InitSocket();
    // 连接服务器
    int ConnectServer(const char* serverIP, int serverPort);
    // 关闭socket
    void CloseSocket();
    // 发送数据
    int SendMsg(DataHead* sendMsg);
    // 接收数据
    int RecvMsg();
    // 客户端运行
    int OnRun();
    // 客户端运行条件
    int IsRun();
    // 处理消息 重写该函数即可
    void ProcessMsg(DataHead* recvMsg);
    // 启动客户端
    void Start(const char* serverIP, int serverPort);
    

private:
    SOCKET sock;
    int ipType;
    int dataStreamType;
    int protocol;
    int portNum;
    char addrIP[IP_ADDR_LEN_MAX];
    int count;

    char recvBuff[RECV_BUFF_SIZE];
    int lastPos; // 记录当前消息缓冲区的大小

};
















#endif





#include <socket_client.h>
#include <sock_p.h>
#include <sock_data.h>
#include <string.h>
#include <iostream>

using namespace std;

/*==============================================
 * description: tcp client构造函数
 *==============================================*/
WTcpClient::WTcpClient() : sock(INVALID_SOCKET), ipType(IPV4), dataStreamType(SOCK_STREAM),
                           protocol(IPPROTO_TCP), portNum(0), count(0), lastPos(0)                                        
{
    memset(addrIP, 0, IP_ADDR_LEN_MAX);
    memset(recvBuff, 0, RECV_BUFF_SIZE);
}

/*==============================================
 * Function name: WTcpClient::InitSocket
 * description: 初始化创建socket
 *==============================================*/
int WTcpClient::InitSocket()
{
    if (sock != INVALID_SOCKET) {
        CloseSocket();
    }
    sock = socket(ipType, dataStreamType, protocol);
    if (sock == INVALID_SOCKET) {
        cout << "creat socket Fail!" << endl;
        return VOS_ERR;
    }

    return VOS_OK;
}

/*==============================================
 * Function name: WTcpClient::ConnectServer
 * description: 连接服务器
 *==============================================*/
int WTcpClient::ConnectServer(const char* serverIP, int serverPort)
{
    if (!IsRun() || (serverIP == nullptr)) {
        return VOS_ERR;
    }
    portNum = serverPort;
    strcpy(addrIP, serverIP);
    sockaddr_in sin = {};
    sin.sin_family = ipType;
    sin.sin_port = htons(portNum);
#ifdef _WIN32
    sin.sin_addr.S_un.S_addr = inet_addr(addrIP);
#else
    sin.sin_addr.s_addr = inet_addr(addrIP);
#endif
    int ret = 0;
    ret = connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in));
    if (ret == SOCKET_ERROR) {
        printf("connect server fail...\n");
        return VOS_ERR;
    }

    printf("connect server(%s) OK...\n", addrIP);
    return VOS_OK;    
}

/*==============================================
 * Function name: WTcpClient::CloseSocket
 * description: 关闭socket
 *==============================================*/
void WTcpClient::CloseSocket()
{
    if (sock == INVALID_SOCKET) {
        return;
    }

#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    cout << "sock is closed ..." << endl;

}

/*==============================================
 * Function name: WTcpClient::SendMsg
 * description: 发送数据,所有的消息都是继承自datahead，所以直接用基类即可
 *==============================================*/
int WTcpClient::SendMsg(DataHead* sendMsg)
{
    if (!IsRun() || (sendMsg == nullptr)) {
        return VOS_ERR;
    }

    int ret = send(sock, (const char*)sendMsg, sendMsg->dataLength, 0);
    if (ret == SOCKET_ERROR) {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*==============================================
 * Function name: WTcpClient::IsRun
 * description: 判断客户端退出条件
 *==============================================*/
int WTcpClient::IsRun()
{
    if (sock == INVALID_SOCKET) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*==============================================
 * Function name: WTcpClient::ProcessMsg
 * description: 接收数据，处理粘包，拆分包
 *==============================================*/
void WTcpClient::ProcessMsg(DataHead* recvMsg)
{
    switch (recvMsg->cmd) {
        case LOG_RET:
        {
            LogInRet* msg = (LogInRet*)recvMsg;
            // cout << "login result is: " << msg->ret << endl;
            break;
        }
        case LOG_OTHER:
        {
            LogInOther* msg = (LogInOther*)recvMsg;
            cout << "sock " << msg->sock << "  join us..." << endl;
            break;
        }
        default:
            cout << "err msg ........" << endl;
            break;
    }
}

/*==============================================
 * Function name: WTcpClient::RecvMsg
 * description: 接收数据，处理粘包，拆分包
 *==============================================*/
int WTcpClient::RecvMsg()
{
    // 通过接收缓冲区不断取消息，然后放到消息缓冲区中等待处理
    char* recvPtr = recvBuff + lastPos;
    int recvLen = recv(sock, recvPtr, RECV_BUFF_SIZE - lastPos, 0);
    if (recvLen <= 0) {
        cout << "server connect fail ..." << endl;
        sock = INVALID_SOCKET;
        return VOS_ERR;
    }

    lastPos += recvLen;
    while (lastPos >= sizeof(DataHead)) {
        DataHead* header = (DataHead*)recvBuff;        
        if (lastPos >= header->dataLength) {
            // 先记录要处理消息的长度
            int dataLength = header->dataLength;
            ProcessMsg(header);
            memcpy(recvBuff, recvBuff + dataLength, lastPos - dataLength);
            lastPos -= dataLength;
        } else {
            break;
        }
    }

    // cout << "get data length is " << recvLen << endl; 

    return VOS_OK;
}

/*==============================================
 * Function name: WTcpClient::OnRun
 * description: 客户端运行
 *==============================================*/
int WTcpClient::OnRun()
{
    while (IsRun()) {
        fd_set fdRead;
        FD_ZERO(&fdRead);
        FD_SET(sock, &fdRead);
        timeval time = {0, 0};
        int ret = select(sock + 1, &fdRead, NULL, NULL, &time);
        // count++;
        // printf("select ret = %d, count = %d\n", ret, count);
        if (ret < 0) {
            cout << "select fail ..." << endl;
            sock == INVALID_SOCKET;
            return VOS_ERR;
        } else if (ret == 0) {
            LogIn login = {};
            login.age = 20;
            strcpy(login.name, "wang fang");
            SendMsg(&login);
        } else {
            if (FD_ISSET(sock, &fdRead)) {
                FD_CLR(sock, &fdRead);
                RecvMsg();
            }
        }
    }
    return VOS_OK;
}

/*==============================================
 * Function name: WTcpClient::Start
 * description: 启动客户端
 *==============================================*/
void WTcpClient::Start(const char* serverIP, int serverPort)
{
    InitSocket();
    ConnectServer(serverIP, serverPort);
    OnRun();
}



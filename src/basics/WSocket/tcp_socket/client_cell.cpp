#include <client_cell.h>
#include <cstdio>   // printf用


/*==============================================
 * Function name: ClientCell::ClientCell
 * description: 
 *==============================================*/
ClientCell::ClientCell(SOCKET sock, sockaddr_in sAddr, INetEvent* eve) :
        cSockfd(sock), cSockAddr(sAddr), recvLastPos(0), sendLastPos(0), event(eve)
{
    memset(recvMsgBuff, 0, RECV_BUFF_SIZE);
    memset(sendMsgBuff, 0, SEND_BUFF_SIZE);
    clientCnt++;
}

ClientCell::~ClientCell()
{
    Close();
    clientCnt--;
}


/*==============================================
 * Function name: ClientCell::IsRun
 * description: 关闭socket
 *==============================================*/
bool ClientCell::IsRun()
{
    return !(cSockfd == INVALID_SOCKET);
}

/*==============================================
 * Function name: ClientCell::Close
 * description: 关闭socket
 *==============================================*/
void ClientCell::Close()
{
    if (cSockfd == INVALID_SOCKET) {
        return;
    }
    #ifdef _WIN32
        closesocket(cSockfd);
    #else
        close(cSockfd);
    #endif

    cSockfd = INVALID_SOCKET;
    return;
}

/*==============================================
 * Function name: ClientCell::SendMsg
 * description: 向客户端发送消息（定时定量发送）,此处需要增加定时发送
 *==============================================*/
bool ClientCell::SendMsg(DataHead* sendMsg)
{
    if (!IsRun() || sendMsg == nullptr) {
        return false;
    }
    int sendLen = ((DataHead*)sendMsg)->dataLength;
    const char* pSend = (const char*)sendMsg;
    // 当缓冲区满的时候一次性发送出去
    while (sendLastPos + sendLen >= SEND_BUFF_SIZE) {
        int copyLen = SEND_BUFF_SIZE - sendLastPos;
        memcpy(sendMsgBuff + sendLastPos, pSend, copyLen);
        pSend += copyLen;
        sendLen -= copyLen;
        sendCnt++;
        if (SOCKET_ERROR == send(cSockfd, sendMsgBuff, SEND_BUFF_SIZE, 0)) {
            return false;
        }
        sendLastPos = 0;
    }
    if (sendLen > 0) {
        memcpy(sendMsgBuff + sendLastPos, pSend, sendLen);
        sendLastPos += sendLen;
    }
    delete sendMsg;
    return true;
}

/*==============================================
 * Function name: ClientCell::RecvMsg
 * description: 接收客户端消息
 *==============================================*/
bool ClientCell::RecvMsg()
{
    if (!IsRun()) {
        return false;
    }
    // 通过接收缓冲区不断取消息，然后放到消息缓冲区中等待处理
    char* recvPtr = recvMsgBuff + recvLastPos;
    int recvLen = recv(cSockfd, recvPtr, RECV_BUFF_SIZE - recvLastPos, 0);
    recvCnt++;
    if (recvLen <= 0) {        
        return false; // 返回err表示客户端已退出
    }
    
    recvLastPos += recvLen;    
    while (recvLastPos >= sizeof(DataHead)) {
        DataHead* header = (DataHead*)recvMsgBuff;        
        if (recvLastPos >= header->dataLength) {
            // 先记录要处理消息的长度
            int dataLength = header->dataLength;

            packCnt++;
            PrcMsg(); // 处理消息

            memcpy(recvMsgBuff, recvMsgBuff + dataLength, recvLastPos - dataLength);
            recvLastPos -= dataLength;
        } else {
            break;
        }
    }
    return true;
}

/*==============================================
 * Function name: ClientCell::PrcMsg
 * description: 处理客户端消息
 *==============================================*/
void ClientCell::PrcMsg()
{
    if (!IsRun()) {
        return;
    }
    DataHead* header = (DataHead*)recvMsgBuff;
    switch (header->cmd)
    {
        case LOG_IN:
        {
            LogIn* logInMsg = (LogIn*)header;
            // printf("socket(%d) login: name is %s, age is %d;\n", (int)cSockfd, 
            //         logInMsg->name, logInMsg->age);
            LogInRet* logInRet = new LogInRet();
            logInRet->ret = 1;
            // event->AddSendTask(this, logInRet);
            SendMsg(logInRet);
            break;
        }
        default:
            printf("recv err msg..............\n");
            break;
    }
    return;
}









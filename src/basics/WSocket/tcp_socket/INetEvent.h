#ifndef _INET_EVENT_TCP_SERVER_
#define _INET_EVENT_TCP_SERVER_

class ClientCell;
// tcp_server的委托事件
class INetEvent
{
public:
    virtual bool IsRun() = 0;                      // server是否运行事件
    virtual void OnLeave(ClientCell* client) = 0;  // 客户端退出事件（客户端退出时会通知Tcpserver,更新自己的客户端列表）
    virtual void AddSendTask(ClientCell* client, void* msg) = 0;  // 向线程池中添加任务

};







#endif


#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <cell_server.h>
#include <INetEvent.h>
#include <thread_pool.h>
#include <client_cell.h>
#include <mutex>

#define SERVER_THREAD_NUM 3

class TcpServer : public INetEvent
{
public:
    TcpServer(const char* serverIP, int portNum);
    ~TcpServer();
    bool Init(); 
    void Stop();

    virtual bool IsRun() override;                      // server是否运行事件
    virtual void OnLeave(ClientCell* client) override;  // 客户端退出事件（客户端退出时会通知Tcpserver,更新自己的客户端列表）
    virtual void AddSendTask(ClientCell* client, void* msg) override;  // 向线程池中添加任务

private:
    bool CreatSocket(); // 创建socket
    bool BindSocket(); // 绑定socket
    bool ListenSocket(); // 监听socket
    void CloseSocket();
    void Accept(); // 接收客户端链接，将客户端添加到客户端队列中

    void Monitor();  //监控线程，测试时使用

private:
    SOCKET sock;              // 服务端sock套接字
    sockaddr_in sockAddr;     // 服务端sock属性
    const int listenNumMax = 20;

    ThreadPool* pool;          // 线程池
    std::vector<CellServer*> servers;  // 子服务端组

    // 此处维护一个客户端数组，可实现一些其他的逻辑需要，且通过事件接口实现客户端数量与几个子服务同步
    std::vector<ClientCell*> clients;  
    std::mutex clientsMtx;   // 锁客户端数组
};









#endif

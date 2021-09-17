#ifndef _TCP_SERVER_CELL_SERVER_H_
#define _TCP_SERVER_CELL_SERVER_H_

#include <vector>
#include <client_cell.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <INetEvent.h>


// 一个服务器有几个cellserver组成，每个cellserver负责一些客户端的处理
class CellServer
{
public:
    CellServer(INetEvent* ser);
    ~CellServer();
    void Run();

    int ClientNum();
    void AddClient(ClientCell* client);

private:
    void MoveClien();
    void ProcessMsg(fd_set* fdRead);
    void Exit();

private:
    INetEvent* event;                      // server的事件接口
    std::vector<ClientCell*> clients;      // 客户端数组
    std::vector<ClientCell*> clientsBuff;  // 客户端数组缓冲区，增加缓冲区，明确资源临界区，更利于操作
    std::atomic_int clientsNum;            // 加入的客户端数量（包括缓冲区）

    SOCKET sockMax;                        // 最大socket
    fd_set fdReadBack;                     // feset备份，当无客户端变化时，直接拷贝，优化性能

    bool isExit;                           // 控制退出
    bool isPrcMsg;                         // 是否还在处理消息

    std::mutex mutexServer;                // 互斥锁
    std::condition_variable isEmpty;       // 客户端数组为空则阻塞线程
};









#endif

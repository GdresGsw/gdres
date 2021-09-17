#include <cell_server.h>
#include <iostream>
#include <thread>
#include <cstdio>   // printf用


/*==============================================
 * Function name: CellServer::CellServer
 * description:
 *==============================================*/
CellServer::CellServer(INetEvent* ser) : event(ser), clientsNum(0), sockMax(INVALID_SOCKET), isExit(false),
                                         isPrcMsg(false)
{
    clients.clear();
    clientsBuff.clear();
    FD_ZERO(&fdReadBack);
}
CellServer::~CellServer()
{
    Exit();
}

/*==============================================
 * Function name: CellServer::ClientNum
 * description: 返回子服务线程管理的客户端的数量
 *==============================================*/
int CellServer::ClientNum()
{
    return clientsNum;
}

/*==============================================
 * Function name: CellServer::AddClient
 * description: 添加客户端(对外访问资源临界区，加锁)
 *==============================================*/
void CellServer::AddClient(ClientCell* client)
{
    clientsNum++;
    std::unique_lock<std::mutex> lck(mutexServer);
    clientsBuff.emplace_back(client);
    isEmpty.notify_all();
    return;
}

/*==============================================
 * Function name: CellServer::MoveClien
 * description: 将缓冲区客户端加入正式客户端数组
 *==============================================*/
void CellServer::MoveClien()
{
    std::unique_lock<std::mutex> lck(mutexServer);
    for (auto it : clientsBuff) {
        clients.emplace_back(it);
        sockMax = sockMax > it->GetSock() ? sockMax : it->GetSock();
        FD_SET(it->GetSock(), &fdReadBack);
    }
    clientsBuff.clear();
}

/*==============================================
 * Function name: CellServer::Run
 * description: 子服务端运行
 *==============================================*/
void CellServer::Run()
{
    fd_set fdRead;
    while (event->IsRun() && !isExit) {
        
        while (clientsBuff.empty() && clients.empty() && event->IsRun() && !isExit)
        {
            std::cout << "cellserver thread wait!" << std::endl;
            std::unique_lock<std::mutex> lck(mutexServer);            
            isEmpty.wait(lck); // 客户端为空时阻塞线程
            if (!event->IsRun() || isExit) {
                printf("CellServer thread exit...\n");
                return;
            }
        }
        
        // 判断缓冲区是否有新加入的客户端
        if (!clientsBuff.empty()) {
            MoveClien();
        }

        FD_ZERO(&fdRead);  // 清空
        memcpy(&fdRead, &fdReadBack, sizeof(fd_set));

        timeval time = {1, 0};
        int ret = select(sockMax + 1, &fdRead, nullptr, nullptr, &time);

        // 小于0表示出现错误,select停止工作
        if (ret < 0) {
            printf("select fail ...\n");
            return;
        }

        // 大于0表示监测到有消息需要处理
        if (ret > 0) {
            ProcessMsg(&fdRead);
        }

        if (ret == 0) {
            // std::cout << "test timeout" << std::endl;
        }
    }

    printf("CellServer thread exit...\n");
    return;
}

/*==============================================
 * Function name: CellServer::ProcessMsg
 * description: 接收并处理客户端消息
 *==============================================*/
void CellServer::ProcessMsg(fd_set* fdRead)
{
    
    if (!event->IsRun() || isExit ) {
        return;
    }

    // 查询socket是否存在可读，处理客户端socket的通信
    int size = clients.size();
    isPrcMsg = true;
    for (auto iter = clients.begin(); iter != clients.end(); iter++) {
        if (FD_ISSET((*iter)->GetSock(), fdRead)) {
            if (!(*iter)->RecvMsg()) {
                printf("socket(%d) exit.\n", (*iter)->GetSock());
                delete *iter;
                clientsNum--;
                event->OnLeave(*iter); // 通知主server有客户端退出
                iter = clients.erase(iter);
            }
            if (iter == clients.end()) {
                break;
            }
        }
    }

    if (size != clients.size()) {
        sockMax = INVALID_SOCKET;
        FD_ZERO(&fdReadBack);
        for (int i = 0; i < clients.size(); i++) {
            FD_SET(clients[i]->GetSock(), &fdReadBack);
            sockMax = sockMax > clients[i]->GetSock() ? sockMax : clients[i]->GetSock();
        }
    }

    isPrcMsg = false;
}

/*==============================================
 * Function name: CellServer::Exit
 * description: 退出
 *==============================================*/
void CellServer::Exit()
{

    isExit = true;
    for (int i = 0; i < clients.size(); i++) {
        clients[i]->Close();
    }

    for (int i = 0; i < clientsBuff.size(); i++) {
        clientsBuff[i]->Close();
    }

    while (isPrcMsg) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    for (int i = 0; i < clients.size(); i++) {
        delete clients[i];
    }

    for (int i = 0; i < clientsBuff.size(); i++) {
        delete clientsBuff[i];
    }
    clients.clear();
    clientsBuff.clear();

    isEmpty.notify_all();
}


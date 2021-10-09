#include <tcp_server.h>
#include <iostream>
#include <wtimer.h>
#include <thread_pool.h>
#include <thread_pool.cpp>


std::atomic_int ClientCell::recvCnt(0);
std::atomic_int ClientCell::packCnt(0);
std::atomic_int ClientCell::sendCnt(0);
std::atomic_int ClientCell::clientCnt(0);

/*==============================================
 * Function name: TcpServer::TcpServer
 * description: 
 *==============================================*/
TcpServer::TcpServer(const char* serverIP, int portNum) : sock(INVALID_SOCKET), pool(nullptr)                              
{
    sockAddr = {};
    sockAddr.sin_family = IPV4; // IP类型
    sockAddr.sin_port = htons(portNum); // host to net unsigned short 主机到网络无符号短整形的转换  端口号
    if (serverIP == nullptr) {
        #ifdef _WIN32
            sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
        #else
            sockAddr.sin_addr.s_addr = INADDR_ANY;
        #endif
    } else {
        #ifdef _WIN32
            sockAddr.sin_addr.S_un.S_addr = inet_addr(serverIP);
        #else
            sockAddr.sin_addr.s_addr = inet_addr(serverIP);
        #endif
    }

    servers.clear();
    clients.clear();
    WStartOrCLeanWinSocket(START_WIN_SOCKET);
}

TcpServer::~TcpServer()
{
    WStartOrCLeanWinSocket(CLEAN_WIN_SOCKET);
}

/*==============================================
 * Function name: TcpServer::CreatSocket
 * description: 初始化创建socket, 默认IPV4 TCP
 *==============================================*/
bool TcpServer::CreatSocket()
{
    if (sock != INVALID_SOCKET) {
        CloseSocket();
    }
    sock = socket(IPV4, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "creat socket Fail!" << std::endl;
        return false;
    }

    return true;
}

/*==============================================
 * Function name: TcpServer::BindSocket
 * description: 绑定socket
 *==============================================*/
bool TcpServer::BindSocket()
{
    if (sock == INVALID_SOCKET) {
        CreatSocket();
    }

    int ret = bind(sock, (sockaddr*)&sockAddr, sizeof(sockaddr_in)); // 如果失败说明端口被占用
    if (ret == SOCKET_ERROR) {
        printf("bind port(%d) fail!\n", sockAddr.sin_port);
        return false;
    }

    return true;
}

/*==============================================
 * Function name: TcpServer::ListenSocket
 * description: 监听socket
 *==============================================*/
bool TcpServer::ListenSocket()
{
    if (sock == INVALID_SOCKET) {
        printf("listen socket(%d) fail!\n", sock);
        return false;
    }
    // 3. listen 监听网络端口
    int ret = listen(sock, listenNumMax);
    if (ret == SOCKET_ERROR) {
        printf("listen socket(%d) fail!\n", sock);
        return false;
    }

    printf("listen socket(%d) OK!\n", sock);
    return true;
}

/*==============================================
 * Function name: TcpServer::IsRun
 * description: 判断服务器运行
 *==============================================*/
bool TcpServer::IsRun()
{
    if (sock == INVALID_SOCKET) {
        return false;
    }

    return true;
}

/*==============================================
 * Function name: TcpServer::CloseSocket
 * description: 关闭socket
 *==============================================*/
void TcpServer::CloseSocket()
{
    if (sock == INVALID_SOCKET) {
        return;
    }

#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    sock = INVALID_SOCKET;

    return;
}

/*==============================================
 * Function name: TcpServer::Accept
 * description: 接收客户端连接线程,将客户端放入客户端列表中
 *==============================================*/
void TcpServer::Accept()
{
    timeval time = {3, 0};
    while (IsRun()) {
        fd_set fdRead; // 有可读
        FD_ZERO(&fdRead);  // 清空 
        FD_SET(sock, &fdRead);
        // 阻塞读取无法退出，需超时读取
        int ret = select(sock + 1, &fdRead, nullptr, nullptr, &time);
        if (ret > 0 && FD_ISSET(sock, &fdRead)) {
            FD_CLR(sock, &fdRead);
            SOCKET cSock = INVALID_SOCKET;
            sockaddr_in clientAddr = {};
            int nAddrLen = sizeof(sockaddr_in); 
        #ifdef _WIN32
            cSock = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);
        #else
            cSock = accept(sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
        #endif

            if (cSock != SOCKET_ERROR) {
                ClientCell* clientSock = new ClientCell(cSock, clientAddr, this);
                if (clientSock != nullptr) {
                    // 将新加入的客户端放入链接客户端数量较少的server
                    int n = 0;
                    int min = servers[n]->ClientNum();
                    for (int i = 0; i < servers.size(); i++) {
                        if (servers[i]->ClientNum() < min) {
                            n = i;
                        }
                    }
                    servers[n]->AddClient(clientSock);
                    std::unique_lock<std::mutex> lck(clientsMtx);
                    clients.emplace_back(clientSock);                    
                }
            }

            continue;
        }

        if (ret < 0) {
            // ----此处需补充失败后的处理逻辑，暂时仅退出接收线程
            printf("accept select fail ...\n");
            sock == INVALID_SOCKET;
            break;
        }

        if (ret == 0) {
            // ---超时处理逻辑
        }
    }

    printf("accept thread exit...\n");
    return;
}

/*==============================================
 * Function name: TcpServer::Stop
 * description: 结束服务器
 *==============================================*/
void TcpServer::Stop()
{
    CloseSocket();
    for (int i = 0; i < servers.size(); i++) {
        delete servers[i];
    }
    delete pool;
    servers.clear();
}

/*==============================================
 * Function name: TcpServer::OnLeave
 * description: 客户端退出通知
 *==============================================*/
void TcpServer::OnLeave(ClientCell* client)
{
    std::unique_lock<std::mutex> lck(clientsMtx);
    
}

/*==============================================
 * Function name: TcpServer::AddTask
 * description: 向线程池中添加任务
 *==============================================*/
void TcpServer::AddSendTask(ClientCell* client, void* msg)
{
    pool->AddTask(std::mem_fn(&ClientCell::SendMsg), client, (DataHead*)msg);
}

/*==============================================
 * Function name: TcpServer::Monitor
 * description: 监控线程（测试用）
 *==============================================*/
void TcpServer::Monitor()
{
    WTimer timer;
    while (IsRun()) {
        if (timer.GetSecond() >= 1) {
            int clientCnt = ClientCell::clientCnt.load(std::memory_order_relaxed);
            int recvCnt = ClientCell::recvCnt.load(std::memory_order_relaxed);
            int packCnt = ClientCell::packCnt.load(std::memory_order_relaxed);
            int sendCnt = ClientCell::sendCnt.load(std::memory_order_relaxed);
            printf("busythreadNum<%d-%d-%d>, time<%.6f>, socket<%d>, clients<%d>, recvCnt<%d>, packCnt<%d>, sendCnt<%d>\n",
            pool->GetBusyNum(), pool->GetLiveNum(), pool->GetTaskNum(), timer.GetSecond(), sock, clientCnt, recvCnt, packCnt, sendCnt);
            timer.update();
            ClientCell::recvCnt = 0;
            ClientCell::packCnt = 0;
            ClientCell::sendCnt = 0;
        }
    }
}

/*==============================================
 * Function name: TcpServer::Init
 * description: server初始化
 *==============================================*/
bool TcpServer::Init()
{
    pool = new ThreadPool(SERVER_THREAD_NUM + 3);
    pool->Init();
    if (CreatSocket() && BindSocket() && ListenSocket()) {
        for (int i = 0; i < SERVER_THREAD_NUM; i++) {
            servers.push_back(new CellServer(this));
        }

        // 在线程池中拉起accept线程
        pool->AddTask(std::mem_fn(&TcpServer::Accept), this);

        // 在线程池中拉起3个子服务端线程
        for (int i = 0; i < SERVER_THREAD_NUM; i++) {
            pool->AddTask(std::mem_fn(&CellServer::Run), servers[i]);
        }

        // 拉起监控线程
        pool->AddTask(std::mem_fn(&TcpServer::Monitor), this);

        wsleeps(1);
        if (pool->GetBusyNum() == SERVER_THREAD_NUM + 3) {
            std::cout << "tcpserver init ok!" << std::endl;
            return true;
        }
    }

    return false;
}


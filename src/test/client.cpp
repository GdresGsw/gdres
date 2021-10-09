#include <socket_client.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <unistd.h>

int main()
{
    WStartOrCLeanWinSocket(START_WIN_SOCKET);
    // int num = 20;
    
    // std::vector<WTcpClient*> clients;
    // for (int i = 0; i < num; i++) {
    //     clients.emplace_back(new WTcpClient);
    // }
    // for (int i = 0; i < num; i++) {
    //     clients[i]->InitSocket();
    //     clients[i]->ConnectServer("192.168.0.106", 4567); // 虚拟机
    //     // clients[i]->ConnectServer("192.168.0.103", 4567); // 本机
    // }

    // LogIn login = {};
    // login.age = 20;
    // strcpy(login.name, "wang fang");
    // while (true) {
    //     for (int i = 0; i < num; i++) {        
    //         clients[i]->SendMsg(&login);
    //     }
    //     // sleep(1);
    // }
    
    WTcpClient client;
    client.Start("192.168.0.103", 4567);
    WStartOrCLeanWinSocket(CLEAN_WIN_SOCKET);
    return 0;
}
#include <tcp_server.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <thread>


int main()
{
    TcpServer server(nullptr, 4567);
    server.Init();
    getchar();
    server.Stop();
    std::cout << "---------" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "---------" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}
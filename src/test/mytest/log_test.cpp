#include "test.h"
#include <iostream>

using namespace gdres;
using namespace std;

// 日志输出测试
void logTest1()
{
    uint64_t start = GetNowTimS();
    while (GetNowTimS() - start <= 10) {
        LOG_SS() << "this is a test rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr " << GetNowTimS() - start;
        LOG("AAAAAAAAAAAAAAAAAAAAAAAAAAA IS %d\n", 123);
    }
    
}

void logTest2()
{
    vector<Thread*> ths;
    ths.reserve(8);
    for (int i = 0; i < 8; i++) {
        ths[i] = new Thread(logTest1, "test" + std::to_string(i));
    }

    for (int i = 0; i < 8; i++) {
        ths[i]->join();
    }
}





void LogTest()
{
    logTest2();
}

// 测试结果：
// 单线程开控制台打印 打印文件：7M/s
// 单线程不开控制台打印 打印文件：15M/s
// 8个线程不开控制台打印 打印文件：8M/s
